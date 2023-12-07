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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"
#include "syn.h"
#include "syntax/gs1syntaxdictionary.h"


#define DEFAULT_SYNTAX_FILENAME "gs1-syntax-dictionary.txt"
#define AI_TABLE_CAPACITY 750
#define MAX_SD_ENTRY_LEN 150


#define error(...) do {							\
	snprintf(ctx->errMsg, sizeof(ctx->errMsg), __VA_ARGS__);	\
	ctx->errFlag = true;						\
	goto fail;							\
} while(0)


static int processComponent(gs1_encoder* const ctx, char* const component, struct aiComponent* const part) {

	const char *token, *p;
	char *saveptr = NULL;
	int numlinters, linter;
	size_t len;

	// First component is the format specification
	token = strtok_r(component, ",", &saveptr);
	p = token;
	len = strlen(p);
	part->opt = (*p == '[') ? OPT : MAN;		// Component is optional
	if (part->opt == OPT) {	// Find the ending ']'
		if (*(p+len-1) != ']')
			error("Format specification for optional component is missing ']': %s", token);
		p++;
		len -= 2;	// Length does not include closing ']'
	}
	switch (*p) {
		case 'X': part->cset = cset_X;    break;
		case 'N': part->cset = cset_N;    break;
		case 'Y': part->cset = cset_Y;    break;
		case 'Z': part->cset = cset_Z;    break;
		case '_': part->cset = cset_none; break;	// Filler
		default:  error("Unknown character set %c", *p);
	}

	if (len < 2)
		error("Format specification for component is too short: %s", token);

	p++; len--;

	if ((*p >= '1' && *p <= '9') ||
	    (part->cset == cset_none && *p == '0')) {		// e.g. X12 or [X12] or "_0"

		if (len > 2) {
			error("AI length too long: %s", token);
		} else if (len == 2) {
			if (*(p+1) < '0' || *(p+1) > '9')
				error("AI length is not a number: %s", token);
			part->min = part->max = (uint8_t)((*p - '0') * 10 + (*(p+1) - '0'));
		} else {			/* len == 1 */
			part->min = part->max = (uint8_t)(*p - '0');
		}

	} else if (len >= 3 && *p == '.' && *(p+1) == '.' &&
		   *(p+2) >= '1' && *(p+2) <= '9' ) {		// e.g. X..20 or [X..20]

		p += 2; len -= 2;

		part->min = 1;
		if (len > 2) {
			error("AI length too long: %s", token);
		} else if (len == 2) {
			if (*(p+1) < '0' || *(p+1) > '9')
				error("AI length is not a number: %s", token);
			part->max = (uint8_t)((*p - '0') * 10 + (*(p+1) - '0'));
		} else {			/* len == 1 */
			part->max = (uint8_t)(*p - '0');
		}

	} else {
		error("Unrecognised format specification for component: %s", token);
	}

	// Remaining tokens are the names of linters
	numlinters = 0;
	while ((token = strtok_r(NULL, ",", &saveptr)) != NULL) {

		if (numlinters >= MAX_LINTERS - 1)
			error("Number of linters for component exceeds implementation: %s", component);

		if ((part->linters[numlinters] = gs1_linter_from_name(token)) == NULL)
			error("Unknown linter '%s'", token);

		numlinters++;

	}
	for (linter = numlinters; linter < MAX_LINTERS; linter++)
		part->linters[linter] = NULL;

	return 0;  // Success

fail:
	return -1;

}

int parseSyntaxDictionaryEntry(gs1_encoder* const ctx, const char* const line, const struct aiEntry* const sd, struct aiEntry** const entry, const uint16_t cap) {

	const struct aiEntry *lastEntry;
	const char *token;
	char *saveptr = NULL;
	char *p, *q;
	size_t len;
	char rangeEnd;
	int numparts, part, linter;
	char buf[MAX_AI_ATTR_LEN + 2] = { 0 };
	char linebuf[MAX_SD_ENTRY_LEN + 1] = { 0 };

	if (strlen(line) > MAX_SD_ENTRY_LEN)
		error("Entry too long");

	// Do nothing with empty and comment-only lines
	strcpy(linebuf, line);
	token = strtok_r(linebuf, " \t", &saveptr);
	if (!token || *token == '#')
		return 0;

	if ((uint16_t)(*entry - sd) >= cap - 1)
		error("Syntax Dictionary capacity is too small");

	*(*entry)->ai = '\0';
	(*entry)->attrs = NULL;
	(*entry)->title = NULL;

	// Initial token should be an AI or an AI range
	len = strlen(token);
	if ((p = strchr(token, '-')) != NULL) {

		if (len < MIN_AI_LEN*2+1 || len > MAX_AI_LEN*2+1)
			error("AI range has wrong width");

		if ((len%2 != 1) || ((size_t)(p - token) != len/2))
			error("AIs in range must have equal width");

		if (strspn(token, "0123456789") != len/2 ||
		    strspn(token + len/2+1, "0123456789") != len/2)
			error("AIs must be numeric");

		if (memcmp(token, token + len/2+1, len/2-1) != 0)
			error("AI range parts may only differ in their last digit");

		if (*(token + len/2-1) >= *(token + len-1))
			error("AI range end must exceed range start");

		memcpy((*entry)->ai, token, len/2);
		(*entry)->ai[len/2] = '\0';

		rangeEnd = *(token + len-1);
		len /= 2;

	} else {

		if (len < MIN_AI_LEN || len > MAX_AI_LEN)
			error("AI has wrong width");

		if (strspn(token, "0123456789") != len)
			error("AI must be numeric");

		strcpy((*entry)->ai, token);
		rangeEnd = *(token + len-1);

	}

	token = strtok_r(NULL, " \t", &saveptr);
	if (!token)
		error("Truncated after AI");

	// Next token may be a "no FNC1" indicator flag
	if (strcmp(token, "*") == 0) {
		(*entry)->fnc1 = NO_FNC1;
		token = strtok_r(NULL, " \t", &saveptr);
		if (!token)
			error("Truncated after flags");
	} else {
		(*entry)->fnc1 = DO_FNC1;
	}

	// Read and process the AI components
	numparts = 0;
	while (token && ((*token >= 'A' && *token <= 'Z') || *token == '[')) {
		if (numparts >= MAX_PARTS - 1)
			error("Number of AI components exceeds implementation");

		if (processComponent(ctx, (char*)token, &(*entry)->parts[numparts]) < 0)
			goto fail;

		numparts++;
		token = strtok_r(NULL, " \t", &saveptr);
	}
	if (numparts == 0)
		error("AI is missing components");
	for (part = numparts; part < MAX_PARTS; part++)
		processComponent(ctx, "_0", &(*entry)->parts[part]);

	// Read the key/value attributes until the title delimiter
	p = buf;
	while (token && strcmp(token, "#") != 0) {

		if ((q = strchr(token, '=')) != NULL) {		// e.g. dlpkey=1,2,3

			if (token == q)
				error("Attribute name required on LHS of assignment");

			*q = '\0';
			if (strspn(token, "abcdefghijklmnopqrstuvwxyz") != strlen(token))
				error("Attribute name contains illegal characters");
			*q = '=';

			if (strspn(q+1, "abcdefghijklmnopqrstuvwxyz0123456789-+_,|") != strlen(q+1))
				error("Attribute value contain illegal characters");

			if (*(q+1) == '\0')
				error("Attribute value required on RHS of assignment");

		} else {					// e.g. dlpkey

			if (strspn(token, "abcdefghijklmnopqrstuvwxyz") != strlen(token))
				error("Singleton attribute name contains illegal characters");

		}

		if ((size_t)(p - buf) + strlen(token) > MAX_AI_ATTR_LEN)
			error("Attributes too long");
		p += snprintf(p, sizeof(buf) - (size_t)(p-buf), "%s ", token);

		token = strtok_r(NULL, " \t", &saveptr);

	}
	if (p != buf)
		*(p-1) = '\0';			// Chop final space
	(*entry)->attrs = strdup(buf);
	if (!(*entry)->attrs)
		error("Failed to allocate memory for attrs");

	// Read until the end of line for the title
	token = strtok_r(NULL, "", &saveptr);
	if (token) {

		if (strspn(token, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890#()-+,./²³ ") != strlen(token))
			error("Title contain illegal characters");

		(*entry)->title = strdup(token);
		if (!(*entry)->title)
			error("Failed to allocate memory for title");

	} else {
		(*entry)->title = strdup("");
		if (!(*entry)->title)
			error("Failed to allocate memory for title");
	}

	// Duplicate the initial entry to fill down to the end of the range
	lastEntry = (*entry)++;
	while (lastEntry->ai[len-1] != rangeEnd) {

		if ((uint16_t)(*entry - sd) >= cap - 1)
			error("Syntax Dictionary capacity is too small");

		strcpy((*entry)->ai, lastEntry->ai);
		(*entry)->ai[len-1]++;
		(*entry)->fnc1 = lastEntry->fnc1;
		for (part = 0; part < MAX_PARTS; part++) {
			(*entry)->parts[part].cset = lastEntry->parts[part].cset;
			(*entry)->parts[part].min  = lastEntry->parts[part].min;
			(*entry)->parts[part].max  = lastEntry->parts[part].max;
			for (linter = 0; linter < MAX_LINTERS; linter++)
				(*entry)->parts[part].linters[linter] = lastEntry->parts[part].linters[linter];
		}
		(*entry)->attrs = strdup(lastEntry->attrs);
		if (!(*entry)->attrs)
			error("Failed to allocate memory for attrs");
		(*entry)->title = strdup(lastEntry->title);
		if (!(*entry)->title)
			error("Failed to allocate memory for title");

		(*entry)++;
		lastEntry++;

	}

	// Prepare the next entry
	*(*entry)->ai = '\0';

	return (int16_t)( (*entry) - sd);

fail:
	*(*entry)->ai = '\0';
	free((*entry)->title);
	(*entry)->title = NULL;
	free((*entry)->attrs);
	(*entry)->attrs = NULL;
	return -1;

}

#undef error


static struct aiEntry* parseSyntaxDictionaryFile(gs1_encoder* const ctx, const char* const fname) {

	const uint16_t cap = AI_TABLE_CAPACITY;
	FILE *fp = NULL;
	char buf[MAX_SD_ENTRY_LEN];
	char errbuf[sizeof(ctx->errMsg)];
	size_t linenum;

	struct aiEntry *sd;
	struct aiEntry *pos;

	sd = (struct aiEntry*)malloc(cap * sizeof(struct aiEntry));
	if (!sd) {
		strcpy(ctx->errMsg, "Failed to allocate AI table");
		ctx->errFlag = true;
		goto fail;
	}
	sd[0].ai[0] = '\0';

	fp = fopen(fname, "r");
	if (fp == NULL) {
		snprintf(ctx->errMsg, sizeof(ctx->errMsg), "Cannot read file %s", fname);
		ctx->errFlag = true;
		goto fail;
	}

	pos = sd;
	linenum = 1;
	while (fgets(buf, sizeof(buf), fp)) {
		buf[strcspn(buf, "\n")] = 0;		/* Chop newline */
		if (parseSyntaxDictionaryEntry(ctx, buf, sd, &pos, cap) < 0) {
			int s = snprintf(errbuf, sizeof(errbuf), "Syntax Dictionary line %d: %s", (int)linenum, ctx->errMsg);
			if (s < (int)sizeof(errbuf))
				memcpy(ctx->errMsg, errbuf, sizeof(errbuf));
			goto fail;
		}
		linenum++;
	}

	fclose(fp);

	return sd;

fail:
	if (fp) fclose(fp);
	gs1_freeSyntaxDictionaryEntries(ctx, sd);
	free(sd);
	return NULL;

}


bool gs1_loadSyntaxDictionary(gs1_encoder* const ctx, const char *fname) {

	struct aiEntry *sd;

	const char* const filename = fname ? fname : DEFAULT_SYNTAX_FILENAME;

	/*
	 * If a name isn't provided then attempt to load the default Syntax Dictionary file.
	 *
	 */
	if ((sd = parseSyntaxDictionaryFile(ctx, filename)) == NULL) {
		printf("\n*** Failed to parse Syntax Dictionary file: %s\n", filename);
		printf("*** %s\n", ctx->errMsg);
	}

	/*
	 *  If parsing failed then we will be calling gs1_setAItable with NULL
	 *  which will load the embedded AI table.
	 *
	 */
	gs1_setAItable(ctx, sd);

	return true;

}

void gs1_freeSyntaxDictionaryEntries(gs1_encoder* const ctx, struct aiEntry *sd) {

	(void)ctx;

	assert(sd);

	while (*sd->ai) {
		*sd->ai = '\0';
		free(sd->attrs);
		sd->attrs = NULL;
		free(sd->title);
		sd->title = NULL;
		sd++;
	}

}


#ifdef UNIT_TESTS

#define TEST_NO_MAIN
#include "acutest.h"

static void test_parseSyntaxDictionaryEntry(gs1_encoder* const ctx, char* const sdEntry, const struct aiEntry expectedAIentries[], bool expectSuccess) {

	const uint16_t cap = 600;
	int16_t numOut, expectOut = 0;
	size_t i, j, k;
	char buf[256];
	struct aiEntry *out, *tmp;

	TEST_CASE(sdEntry);

	TEST_ASSERT((out = calloc(cap, sizeof(struct aiEntry))) != NULL);
	assert(out);

	*out->ai = '\0';
	tmp = out;
	strcpy(buf, sdEntry);

	numOut = (int16_t)parseSyntaxDictionaryEntry(ctx, buf, out, &tmp, cap);

	if (!expectSuccess) {
		TEST_CHECK(numOut == -1);
		TEST_MSG("Expacted failure. Got %d aiEntries", numOut);
		goto out;
	}

	TEST_CHECK(numOut >= 0);
	TEST_MSG("Expected success. Got error: %s", ctx->errMsg);
	if (numOut == -1)
		goto out;

	while (*expectedAIentries[expectOut].ai)
		expectOut++;

	TEST_CHECK(numOut == expectOut);
	TEST_MSG("Got %d aiEntries; Expected %d", numOut, expectOut);

	/*
	 * Compare the aiEntries received from the parsed output with the expect
	 * output
	 *
	 */
	for (i = 0; i < (size_t)expectOut && i < (size_t)numOut; i++) {
		TEST_CHECK(strcmp(out[i].ai, expectedAIentries[i].ai) == 0);
		TEST_CHECK(out[i].fnc1 == expectedAIentries[i].fnc1);
		for (j = 0; j < MAX_PARTS; j++) {
			TEST_CHECK(out[i].parts[j].cset == expectedAIentries[i].parts[j].cset);
			TEST_CHECK(out[i].parts[j].min  == expectedAIentries[i].parts[j].min );
			TEST_CHECK(out[i].parts[j].max  == expectedAIentries[i].parts[j].max );
			for (k = 0; k < MAX_LINTERS; k++)
				TEST_CHECK(out[i].parts[j].linters[k] == expectedAIentries[i].parts[j].linters[k]);
		}
		TEST_CHECK(strcmp(out[i].attrs, expectedAIentries[i].attrs) == 0);
		TEST_CHECK(strcmp(out[i].title, expectedAIentries[i].title) == 0);
	}

out:
	gs1_freeSyntaxDictionaryEntries(ctx, out);
	free(out);

}


struct test_parse_sd_entry_s {
	bool expectSuccess;
	char* sdEntry;
	struct aiEntry aiEntries[11];
};

struct test_parse_sd_entry_s tests_parse_sd_entry[] = {
	{ true,  "",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "#",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# ",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# COMMENT",			{ AI_ENTRY_TERMINATOR } },
	{ true, "90  X..30  # INTERNAL", {
		AI_ENTRY("90", DO_FNC1, X,1,30,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "91-99  X..90  # INTERNAL", {
		AI_ENTRY("91", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("92", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("93", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("94", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("95", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("96", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("97", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("98", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("99", DO_FNC1, X,1,90,MAN,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8010  Y..30,key  dlpkey=8011  # CPID", {
		AI_ENTRY("8010", DO_FNC1, Y,1,30,MAN,key,_, __, __, __, __, "dlpkey=8011", "CPID"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "253  N13,csum,key [X..17]  dlpkey  # GDTI", {
		AI_ENTRY("253", DO_FNC1, N,13,13,MAN,csum,key, X,1,17,OPT,_,_, __, __, __, "dlpkey", "GDTI"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "7007  N6,yymmdd [N..6],yymmdd  req=01,02  # HARVEST DATE", {
		AI_ENTRY("7007", DO_FNC1, N,6,6,MAN,yymmdd,_, N,1,6,MAN,yymmdd,_, __, __, __, "req=01,02", "HARVEST DATE"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8111  N4  req=255  # POINTS", {
		AI_ENTRY("8111", DO_FNC1, N,4,4,MAN,_,_, __, __, __, __, "req=255", "POINTS"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "01  *  N14,csum,key  ex=02,255,37  dlpkey=22,10,21|235  # GTIN", {
		AI_ENTRY("01", NO_FNC1, N,14,14,MAN,csum,key, __, __, __, __, "ex=02,255,37 dlpkey=22,10,21|235", "GTIN"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8001  N4,nonzero N5,nonzero N3,nonzero N1,winding N1  req=01  # DIMENSIONS", {
		AI_ENTRY("8001", DO_FNC1, N,4,4,MAN,nonzero,_, N,5,5,MAN,nonzero,_, N,3,3,MAN,nonzero,_, N,1,1,MAN,winding,_, N,1,1,MAN,_,_, "req=01", "DIMENSIONS"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8110  X..70,couponcode", {
		AI_ENTRY("8110", DO_FNC1, X,1,70,MAN,couponcode,_, __, __, __, __, "", ""),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "90  X..30  req=999", {		/* Bespoke test for attrs and no title */
		AI_ENTRY("90", DO_FNC1, X,1,30,MAN,_,_, __, __, __, __, "req=999", ""),
		AI_ENTRY_TERMINATOR
	} },
};

void test_syn_parseSyntaxDictionaryEntry(void) {

	size_t i;

	gs1_encoder* ctx;
	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	for (i = 0; i < SIZEOF_ARRAY(tests_parse_sd_entry); i++)
		test_parseSyntaxDictionaryEntry(ctx, tests_parse_sd_entry[i].sdEntry, tests_parse_sd_entry[i].aiEntries, tests_parse_sd_entry[i].expectSuccess);

	gs1_encoder_free(ctx);

}

#endif  /* UNIT_TESTS */

