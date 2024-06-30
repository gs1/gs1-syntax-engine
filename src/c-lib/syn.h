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

#ifndef SYN_H
#define SYN_H

#include <stdbool.h>
#include <stdint.h>

#include "gs1encoders.h"


struct aiEntry;


bool gs1_loadSyntaxDictionary(gs1_encoder *ctx, const char *fname);
void gs1_freeSyntaxDictionaryEntries(const gs1_encoder *ctx, struct aiEntry *sd);

// Exposed for fuzzing
int parseSyntaxDictionaryEntry(gs1_encoder *ctx, const char* line, const struct aiEntry *sd, struct aiEntry **entry, uint16_t cap);


#ifdef UNIT_TESTS

void test_syn_parseSyntaxDictionaryEntry(void);

#endif


#endif  /* SYN_H */
