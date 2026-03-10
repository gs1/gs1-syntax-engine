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
#include "syn.h"

int LLVMFuzzerTestOneInput(const uint8_t* const buf, size_t len) {

	static gs1_encoder *ctx = NULL;
	char in[MAX_DATA+50];
	struct aiEntry sd[150];
	struct aiEntry *tmp = sd;
	int numOut;
	int e;

	if (!ctx) {
		ctx = gs1_encoder_init(NULL);
		assert(ctx);
	}

	if (len > MAX_DATA+49)
		return 0;

	memcpy(in, buf, len);
	in[len] = '\0';

	numOut = parseSyntaxDictionaryEntry(ctx, in, sd, &tmp, sizeof(sd) / sizeof(sd[0]));

	// Structural invariants for successfully-parsed entries
	for (e = 0; e < numOut; e++) {
		int p;

		// AI length must be within bounds
		if (sd[e].ailen < MIN_AI_LEN || sd[e].ailen > MAX_AI_LEN) {
			printf("\nailen=%d out of range for entry %d\n", sd[e].ailen, e);
			abort();
		}

		// AI string must match stated length
		if (strlen(sd[e].ai) != sd[e].ailen) {
			printf("\nstrlen(ai)=%zu != ailen=%d for entry %d\n",
				strlen(sd[e].ai), sd[e].ailen, e);
			abort();
		}

		for (p = 0; sd[e].parts[p].cset != cset_none; p++) {

			// Component min must not exceed max
			if (sd[e].parts[p].min > sd[e].parts[p].max) {
				printf("\nmin=%d > max=%d in part %d of entry %d\n",
					sd[e].parts[p].min, sd[e].parts[p].max, p, e);
				abort();
			}

			// Character set must be valid
			if (sd[e].parts[p].cset > cset_Z) {
				printf("\ncset=%d invalid in part %d of entry %d\n",
					sd[e].parts[p].cset, p, e);
				abort();
			}
		}

		// Component count must not exceed limit
		if (p >= MAX_PARTS) {
			printf("\n%d parts exceeds MAX_PARTS for entry %d\n", p, e);
			abort();
		}
	}

	gs1_freeSyntaxDictionaryEntries(ctx, sd);

	return 0;

}
