/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2021-2026 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 *  This harness fuzzes a complete custom Syntax Dictionary: the input is loaded
 *  as the active AI table and the engine is then driven against it. This covers
 *  the per-line parser, the cross-entry loader invariants (sortedness,
 *  uniqueness), and the downstream use of a hostile table.
 *
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"
#include "ai.h"
#include "dl.h"
#include "syn.h"


// Exercise the major dictionary-dependent output paths for one AI data string
static void drive(gs1_encoder* const ctx, const char* const aidata) {

	char **hri;

	if (!gs1_encoder_setAIdataStr(ctx, aidata))
		return;

	gs1_encoder_getAIdataStr(ctx);
	gs1_encoder_getHRI(ctx, &hri);
	gs1_encoder_getDLuri(ctx, NULL);

}


int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	static gs1_encoder *ctx = NULL;
	FILE *fp;
	struct aiEntry *sd;
	const struct aiEntry *e;
	unsigned int cfg = 0;
	size_t i;

	if (!ctx) {
		ctx = gs1_encoder_init_ex(NULL, NULL);
		assert(ctx);
	}

	if (len < 1 || len > 512*1024)
		return -1;

	/*
	 *  Load the fuzzed dictionary from memory and install it as the active AI
	 *  table. A malformed dictionary is rejected at load; an inconsistent one
	 *  causes setAItable to revert to the embedded table (freeing sd). Either
	 *  way there is nothing further to exercise.
	 *
	 */
	fp = fmemopen((void *)(uintptr_t)buf, len, "r");
	if (!fp)
		return -1;
	sd = gs1_loadSyntaxDictionaryFromFile(ctx, fp);
	fclose(fp);
	if (!sd)
		return -1;

	// We reuse one ctx across runs; setAItable is call-once, so release the
	// key-qualifiers it populated on the previous load before re-invoking it
	gs1_freeDLkeyQualifiers(ctx);
	if (!gs1_setAItable(ctx, sd))
		return -1;

	/*
	 *  Per-entry structural invariants: even on hostile input the parser must
	 *  never produce entries that violate these contracts.
	 *
	 */
	for (e = ctx->aiTable; *e->ai; e++) {

		int p;

		if (e->ailen < MIN_AI_LEN || e->ailen > MAX_AI_LEN) {
			fprintf(stderr, "\nailen=%d out of range for AI '%s'\n", e->ailen, e->ai);
			__builtin_trap();
		}

		if (strlen(e->ai) != e->ailen) {
			fprintf(stderr, "\nstrlen(ai)=%zu != ailen=%d for AI '%s'\n",
				strlen(e->ai), e->ailen, e->ai);
			__builtin_trap();
		}

		for (p = 0; p < MAX_PARTS && e->parts[p].cset != cset_none; p++) {

			if (e->parts[p].min > e->parts[p].max) {
				fprintf(stderr, "\nmin=%d > max=%d in part %d of AI '%s'\n",
					e->parts[p].min, e->parts[p].max, p, e->ai);
				__builtin_trap();
			}

			if (e->parts[p].cset > cset_Z) {
				fprintf(stderr, "\ncset=%d invalid in part %d of AI '%s'\n",
					e->parts[p].cset, p, e->ai);
				__builtin_trap();
			}

		}

		if (p >= MAX_PARTS) {
			fprintf(stderr, "\n%d parts exceeds MAX_PARTS for AI '%s'\n", p, e->ai);
			__builtin_trap();
		}

	}

	// Derive configuration from input content without consuming any bytes
	for (i = 0; i < len; i++)
		cfg = cfg * 31 + buf[i];
	gs1_encoder_setPermitUnknownAIs(ctx, cfg & 1);
	gs1_encoder_setIncludeDataTitlesInHRI(ctx, (cfg >> 1) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, (cfg >> 2) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vMUTEX_AIS, (cfg >> 3) & 1);

	// Fixed, representative inputs against the fuzzed table
	drive(ctx, "(01)12345678901231(10)ABC123(21)XYZ(8200)https://a.example/x");
	drive(ctx, "(3100)123456(3925)12599(00)123456789012345675");

	/*
	 *  Build a data string from the table's own AIs so their lengths, linters
	 *  and attributes are exercised. Digits satisfy every CSET structurally, so
	 *  components are filled with their minimum length of digits.
	 *
	 */
	{
		char data[MAX_DATA+1];
		size_t n = 0;
		int count = 0;

		for (e = ctx->aiTable; *e->ai && count < MAX_AIS; e++, count++) {

			char piece[2 + MAX_AI_LEN + MAX_PARTS*40 + 1];
			size_t pn = 0;
			int p;

			piece[pn++] = '(';
			memcpy(piece + pn, e->ai, e->ailen);
			pn += e->ailen;
			piece[pn++] = ')';

			for (p = 0; p < MAX_PARTS && e->parts[p].cset != cset_none; p++) {
				int L = e->parts[p].min ? e->parts[p].min : 1;
				if (L > 40)
					L = 40;
				while (L-- > 0)
					piece[pn++] = '1';
			}

			if (n + pn >= sizeof(data))
				break;
			memcpy(data + n, piece, pn);
			n += pn;

		}
		data[n] = '\0';

		drive(ctx, data);
	}

	// Scan-data parsing (AI syntax and DL URI) against the fuzzed table
	gs1_encoder_setScanData(ctx, "]d2(01)12345678901231(21)ABC");
	gs1_encoder_setScanData(ctx, "]d1https://id.gs1.org/01/12345678901231/21/XYZ");

	return 0;

}
