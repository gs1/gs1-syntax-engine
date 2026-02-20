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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"

int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	static gs1_encoder *ctx = NULL;
	char in[MAX_DATA+50];
	char pristine[MAX_DATA+50];
	char out1[MAX_DATA+50];
	const char *out;
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
	gs1_encoder_setIncludeDataTitlesInHRI(ctx, (cfg >> 2) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, (cfg >> 3) & 1);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR, (cfg >> 4) & 1);
	gs1_encoder_setAddCheckDigit(ctx, (cfg >> 7) & 1);

	if (len > MAX_DATA+49)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	memcpy(pristine, in, len);

	if (!gs1_encoder_setScanData(ctx, in))
		return 0;

	// Test that the input hasn't been corrupted
	if (memcmp(in, pristine, len) != 0) {
		printf("\n:IN %s\nPRISTINE: %s\n", in, pristine);
		abort();
	};

	out = gs1_encoder_getScanData(ctx);
	if (out == NULL)
		return 0;
	if ((cfg >> 7) & 1) {
		// addCheckDigit may modify data; verify output is stable
		memcpy(out1, out, strlen(out) + 1);
		if (!gs1_encoder_setScanData(ctx, out1))
			abort();
		out = gs1_encoder_getScanData(ctx);
		if (out == NULL)
			abort();
		if (strcmp(out1, out) != 0) {
			printf("\nOUT1: %s\nOUT2: %s\n", out1, out);
			abort();
		}
	} else if (strcmp(in, out) != 0) {
		printf("\nIN:   %s\nOUT:  %s\nSYM:  %d\nDATA: %s\n", in, out, gs1_encoder_getSym(ctx), gs1_encoder_getDataStr(ctx));
		abort();
	}

	return 0;

}
