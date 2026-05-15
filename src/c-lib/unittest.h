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

#include "gs1encoders.h"


/*
 *  GS1_ENCODER_UNIT_TEST_INIT(): convenience initialiser that attempts
 *  to load gs1-syntax-dictionary.txt from the current working directory
 *  and falls back to the embedded AI table on any failure. Mirrors the
 *  legacy gs1_encoder_init() autoload behaviour. CI exercises both code
 *  paths by running the test suite with and without the file present.
 *
 */
#define GS1_ENCODER_UNIT_TEST_INIT() gs1_encoder_init_ex(NULL,			\
	&(gs1_encoder_init_opts_t){						\
		.struct_size      = sizeof(gs1_encoder_init_opts_t),		\
		.flags            = gs1_encoder_iFALLBACK_ON_SYNDICT_ERROR,	\
		.syntaxDictionary = "gs1-syntax-dictionary.txt",		\
	})


#endif
