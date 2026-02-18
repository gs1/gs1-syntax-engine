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
#include <stdio.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"
#include "syn.h"

int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	static gs1_encoder *ctx = NULL;
	char in[MAX_DATA+50];
	struct aiEntry sd[150];
	struct aiEntry *tmp = sd;

	if (!ctx) {
		ctx = gs1_encoder_init(NULL);
		assert(ctx);
	}

	if (len > MAX_DATA+49)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	parseSyntaxDictionaryEntry(ctx, in, sd, &tmp, sizeof(sd) / sizeof(sd[0]));
	gs1_freeSyntaxDictionaryEntries(ctx, sd);

	return 0;

}
