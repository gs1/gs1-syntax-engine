/**
 * GS1 Syntax Engine
 *
 * @author Copyright (c) 2021-2024 GS1 AISBL.
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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"

static gs1_encoder *ctx = NULL;


int LLVMFuzzerInitialize(int *argc, char ***argv) {

	(void)argc;
	(void)argv;

	ctx = gs1_encoder_init(NULL);
	gs1_encoder_setPermitUnknownAIs(ctx, true);
	gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, true);

	return 0;

}


int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	char in[MAX_DATA+1];
	char outDL1[MAX_DATA+1];
	char outDL2[MAX_DATA+1];
	const char *out;

	if (len > MAX_DATA)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	/*
	 *  Pour random data in, then progress inputs that result in a valid DL
	 *  URI
	 *
	 */
	if (!gs1_encoder_setDataStr(ctx, in))
		return -1;

	if ((out = gs1_encoder_getDLuri(ctx, NULL)) == NULL)
		return -1;
	strcpy(outDL1, out);

	/*
	 *  Put the DL URI back in and pull it out to ensure that we receive
	 *  the original DL uri
	 *
	 */
	if (!gs1_encoder_setDataStr(ctx, outDL1)) {
		printf("\nFailed setting data to DL: %s\n", outDL1);
		printf("\nError: %s\n", ctx->errMsg);
		abort();
	}

	if ((out = gs1_encoder_getDLuri(ctx, NULL)) == NULL) {
		printf("\nFailed reading DL after successfully setting: %s\n", outDL1);
		abort();
	}
	strcpy(outDL2, out);

	if (strcmp(outDL1, outDL2) != 0) {
		printf("\nIN:  %s\nOUT: %s\n", outDL1, outDL2);
		abort();
	}

	return 0;

}
