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

	return 0;

}


int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	char in[MAX_DATA+1];
	char pristine[MAX_DATA+1];
	const char *out;

	if (len > MAX_DATA)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	// Strip trailing GS characters from the input
	while (strlen(in) > 0 && in[strlen(in)-1] == 0x1D)
		in[strlen(in)-1] = '\0';

	memcpy(pristine, in, len);

	if (!gs1_encoder_setScanData(ctx, in))
		return 0;

	// Test that the input hasn't been corrupted
	if (memcmp(in, pristine, len) != 0) {
		printf("\n:IN %s\nPRISTINE: %s\n", in, pristine);
		abort();
	};

	out = gs1_encoder_getScanData(ctx);
	if (strcmp(in, out) != 0) {
		printf("\nIN:   %s\nOUT:  %s\nSYM:  %d\nDATA: %s\n", in, out, gs1_encoder_getSym(ctx), gs1_encoder_getDataStr(ctx));
		abort();
	}

	return 0;

}
