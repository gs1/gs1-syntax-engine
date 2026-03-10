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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"

int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	static gs1_encoder *ctx = NULL;

	static const char * const stems[] = {
		NULL,                    /* canonical stem                */
		"https://example.com",   /* short stem, no trailing slash */
		"https://example.com/",  /* trailing slash trim path      */
		"https://a",             /* minimal stem                  */
	};

	char in[MAX_DATA+50];
	char outDL1[MAX_DATA+1];
	char outDL2[MAX_DATA+1];
	char copybuf[256];
	const char *out;
	const char *stem;
	char **hri;
	char **qp;
	unsigned int cfg = 0;
	size_t i;

	if (!ctx) {
		ctx = gs1_encoder_init(NULL);
		assert(ctx);
	}

	/*
	 *  Derive configuration from input content so that the fuzzer
	 *  explores the space of configuration flags without consuming
	 *  any input bytes (which would break auto-extracted seeds).
	 *
	 */
	for (i = 0; i < len; i++)
		cfg = cfg * 31 + buf[i];
	gs1_encoder_setPermitUnknownAIs(ctx, cfg & 1);
	gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, (cfg >> 1) & 1);
	gs1_encoder_setIncludeDataTitlesInHRI(ctx, (cfg >> 2) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, (cfg >> 3) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR, (cfg >> 4) & 1);
	stem = stems[(cfg >> 5) & 3];
	gs1_encoder_setAddCheckDigit(ctx, (cfg >> 7) & 1);

	if (len > MAX_DATA+49)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	/*
	 *  Feed the input to setDataStr. Inputs beginning with http:// or
	 *  https:// dispatch to gs1_parseDLuri for DL URI parsing and AI
	 *  extraction. The seed corpus for this fuzzer is naturally rich
	 *  in DL URIs since the quoted strings extracted from dl.c contain
	 *  100+ test URIs.
	 *
	 */
	if (!gs1_encoder_setDataStr(ctx, in))
		return 0;

	/*
	 *  The input was accepted, so exercise the output paths.
	 *
	 */

	// AI data must be extractable when AIs were parsed
	if (ctx->numAIs > 0 && gs1_encoder_getAIdataStr(ctx) == NULL) {
		printf("\ngetAIdataStr NULL with numAIs=%d after: %s\n", ctx->numAIs, in);
		abort();
	}

	gs1_encoder_getHRI(ctx, &hri);
	gs1_encoder_getDLignoredQueryParams(ctx, &qp);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	gs1_encoder_copyHRI(ctx, copybuf, (cfg >> 8) & 0xFF);
	gs1_encoder_copyDLignoredQueryParams(ctx, copybuf, (cfg >> 8) & 0xFF);
#pragma GCC diagnostic pop

	/*
	 *  Validate the DL URI round trip: generate a DL URI from the
	 *  parsed data then feed it back and check stability.
	 *
	 *  Skip the round trip when addCheckDigit is enabled because
	 *  the check digit modification on the first pass makes the
	 *  generated DL URI differ from what a second pass produces.
	 *
	 */
	if ((out = gs1_encoder_getDLuri(ctx, stem)) == NULL)
		return 0;
	if (!((cfg >> 7) & 1)) {
		memcpy(outDL1, out, strlen(out) + 1);

		if (!gs1_encoder_setDataStr(ctx, outDL1)) {
			printf("\nFailed setting data to DL: %s\n", outDL1);
			printf("\nError: %s\n", ctx->errMsg);
			abort();
		}

		if ((out = gs1_encoder_getDLuri(ctx, stem)) == NULL) {
			printf("\nFailed reading DL after successfully setting: %s\n", outDL1);
			abort();
		}
		memcpy(outDL2, out, strlen(out) + 1);

		if (strcmp(outDL1, outDL2) != 0) {
			printf("\nIN:  %s\nOUT: %s\n", outDL1, outDL2);
			abort();
		}
	}

	return 0;

}
