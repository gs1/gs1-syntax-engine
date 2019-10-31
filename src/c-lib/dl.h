/**
 * GS1 Syntax Engine
 *
 * @author Copyright (c) 2021-2022 GS1 AISBL.
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

#ifndef DL_H
#define DL_H

#include <stdbool.h>
#include <stdint.h>

#include "gs1encoders.h"


#define DL_PATH_ORDER_ATTRIBUTE		UINT8_MAX


bool gs1_populateDLkeyQualifiers(gs1_encoder *ctx);
void gs1_freeDLkeyQualifiers(gs1_encoder *ctx);
bool gs1_parseDLuri(gs1_encoder *ctx, char *dlData, char *dataStr);
char* gs1_generateDLuri(gs1_encoder* ctx, const char* stem);


#ifdef UNIT_TESTS

void test_dl_testValidateDLpathAIseq(void);
void test_dl_parseDLuri(void);
void test_dl_URIunescape(void);
void test_dl_URIescape(void);
void test_dl_generateDLuri(void);

#endif


#endif  /* DL_H */
