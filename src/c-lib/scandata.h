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

#ifndef SCANDATA_H
#define SCANDATA_H


#include "enc-private.h"

char* gs1_generateScanData(gs1_encoder *ctx);
bool gs1_processScanData(gs1_encoder* ctx, const char* scanData);


#ifdef UNIT_TESTS

void test_scandata_validateParity(void);
void test_scandata_generateScanData(void);
void test_scandata_processScanData(void);

#endif


#endif  /* SCANDATA_H */
