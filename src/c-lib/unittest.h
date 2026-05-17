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

#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdlib.h>

#include "gs1encoders.h"


/*
 *  Convenience initialiser used by the test binaries. The AI-table source
 *  is selected by the GS1_TEST_SYNDICT env var: when set to a non-empty
 *  path the library loads that Syntax Dictionary file (with fallback to
 *  the embedded table on read failure); when unset or empty the library
 *  uses the embedded table directly. CI exercises both paths by running
 *  the same test binary twice with and without the env var set.
 *
 */
static inline gs1_encoder *gs1_encoder_unit_test_init(void) {
	const char *syndict = getenv("GS1_TEST_SYNDICT");
	gs1_encoder_init_opts_t opts = {
		.struct_size      = sizeof(gs1_encoder_init_opts_t),
		.flags            = gs1_encoder_iFALLBACK_ON_SYNDICT_ERROR,
		.syntaxDictionary = (syndict && *syndict) ? syndict : NULL,
	};
	return gs1_encoder_init_ex(NULL, &opts);
}


#endif
