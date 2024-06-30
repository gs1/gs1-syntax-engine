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

#ifndef AI_H
#define AI_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "syntax/gs1syntaxdictionary.h"

#define MAX_AIS			64
#define MIN_AI_LEN		2
#define MAX_AI_LEN		4
#define MAX_AI_VALUE_LEN	90
#define MAX_AI_ATTR_LEN		64


/*
 * If these are redefined then the macro expansions for the embedded AI table
 * will need to be changed.
 *
 * One more that last entry since we NULL terminate.
 *
 */
#define MAX_PARTS (5 + 1)		/* Currently AI (8001) = N4 N5 N3 N1 N1 */
#define MAX_LINTERS (2 + 1)		/* Many AIs have, e.g. csum,key         */


typedef enum {
	cset_none = 0,
	cset_X,					// CSET 82
	cset_N,					// Numeric
	cset_Y,					// CSET 39
	cset_Z,					// URI-safe base64
} cset_t;


// A single AI may consist of multiple concatenated components
struct aiComponent {
	cset_t cset;				// CSET table
	uint8_t min;				// Minimum length. Currently 0 for optional, 1 for mandatory
	uint8_t max;				// Maximum length
	bool opt;				// Optional final component
	gs1_linter_t linters[MAX_LINTERS];	// Linter functions applied to component
};


struct aiEntry {
	char ai[MAX_AI_LEN+1];			// AI itself
	bool fnc1;				// FNC1 required as a separator
	uint8_t dlDataAttr;			// Permitted as a GS1 DL URI data attribute
	struct aiComponent parts[MAX_PARTS];	// Format specification components
	char* attrs;				// Key-value pair attributes, e.g. req, ex, etc.
	char* title;				// Data title
};


typedef enum {
	aiValue_undef = 0,
	aiValue_aival,				// Extracted AI value pair
	aiValue_ccsep,				// Separater between linear and composite component AIs
	alValue_dlign,				// An ignored (non-numeric) query parameter of a DL URI (stored undecoded in aiValue.value)
} aiValueKind_t;

struct aiValue {
	const struct aiEntry *aiEntry;		// Entry in AI table; may be an "unknown" placeholder entry
	const char *ai;				// Start of the AI in the underlying buffer
	uint8_t ailen;				// Length of the AI
	const char *value;			// Start of the AI value in the underlying buffer
	uint8_t vallen;				// Length of the AI value
	aiValueKind_t kind;			// Kind of AI value
	uint8_t dlPathOrder;			// Denotes the position in a DL URI path component
};


// Features such as validation functions, some of which can be toggled
typedef bool (*gs1_encoder_validation_func_t)(gs1_encoder *ctx);

struct validationEntry {
	bool locked;				// Feature cannot be enabled/disabled via the API
	bool enabled;				// Feature is enabled
	gs1_encoder_validation_func_t fn;	// Optional validation function for the feature
};


/*
 * Syntactic sugar for initialising an aiEntry
 *
 */
#define DO_FNC1 true
#define NO_FNC1 false

#define NO_DATA_ATTR 0
#define DL_DATA_ATTR 1
#define XX_DATA_ATTR 2		// For unknown AIs, depends on gs1_encoder_vUNKNOWN_AI_DL_ATTR

#define MAN false
#define OPT true


#define AI_VA(a, f, d, c1,mn1,mx1,o1,l00,l01, c2,mn2,mx2,o2,l10,l11, c3,mn3,mx3,o3,l20,l21, c4,mn4,mx4,o4,l30,l31, c5,mn5,mx5,o5,l40,l41, k, t) {	\
		.ai = a,																\
		.fnc1 = f,																\
		.dlDataAttr = d,															\
		.parts = {																\
			{ .cset = cset_##c1, .min = mn1, .max = mx1, .opt = o1, .linters = { gs1_lint_##l00, gs1_lint_##l01, NULL } },			\
			{ .cset = cset_##c2, .min = mn2, .max = mx2, .opt = o2, .linters = { gs1_lint_##l10, gs1_lint_##l11, NULL } },			\
			{ .cset = cset_##c3, .min = mn3, .max = mx3, .opt = o3, .linters = { gs1_lint_##l20, gs1_lint_##l21, NULL } },			\
			{ .cset = cset_##c4, .min = mn4, .max = mx4, .opt = o4, .linters = { gs1_lint_##l30, gs1_lint_##l31, NULL } },			\
			{ .cset = cset_##c5, .min = mn5, .max = mx5, .opt = o5, .linters = { gs1_lint_##l40, gs1_lint_##l41, NULL } },			\
			{ .cset = 0,         .min = 0,   .max = 0,   .opt = 0,   .linters = { NULL,       NULL,       NULL } },				\
		},																	\
		.attrs = k,																\
		.title = t,																\
	}
#define PASS_ON(...) __VA_ARGS__
#define AI_ENTRY(...) PASS_ON(AI_VA(__VA_ARGS__))
#define cset_0 0
#define gs1_lint__ NULL
#define __ 0,0,0,0,_,_		/* NULL placeholder instead of e.g. X,1,30,MAN,csum,key */
#define AI_ENTRY_TERMINATOR AI_ENTRY( "", 0, 0, __, __, __, __, __, "", "" )


// Write to unbracketed AI dataStr checking for overflow
#define writeDataStr(v) do {						\
	if (strlen(dataStr) + strlen(v) > MAX_DATA)			\
		goto fail;						\
	strcat(dataStr, v);						\
} while (0)

#define nwriteDataStr(v,l) do {						\
	if (strlen(dataStr) + l > MAX_DATA)				\
		goto fail;						\
	strncat(dataStr, v, l);						\
} while (0)


#include "gs1encoders.h"

void gs1_setAItable(gs1_encoder *ctx, struct aiEntry *table);
const struct aiEntry* gs1_lookupAIentry(const gs1_encoder *ctx, const char *p, size_t ailen);
bool gs1_aiValLengthContentCheck(gs1_encoder *ctx, const char *ai, const struct aiEntry *entry, const char *aiVal, size_t vallen);
bool gs1_parseAIdata(gs1_encoder *ctx, const char *aiData, char *dataStr);
bool gs1_processAIdata(gs1_encoder *ctx, const char *dataStr, bool extractAIs);
bool gs1_validateAIs(gs1_encoder* ctx);
void gs1_loadValidationTable(gs1_encoder* ctx);


#ifdef UNIT_TESTS

void test_ai_lookupAIentry(void);
void test_ai_checkAIlengthByPrefix(void);
void test_ai_AItableVsPrefixLength(void);
void test_ai_AItableVsIsFNC1required(void);
void test_ai_parseAIdata(void);
void test_ai_linters(void);
void test_ai_processAIdata(void);
void test_ai_validateAIs(void);
void test_ai_lint_csumalpha(void);

#endif


#endif  /* AI_H */
