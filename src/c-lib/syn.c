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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>					// IWYU pragma: keep
#include <string.h>

#include "gs1encoders.h"
#include "enc-private.h"
#include "syn.h"
#include "tr.h"
#include "syntax/gs1syntaxdictionary.h"


#define DEFAULT_SYNTAX_FILENAME "gs1-syntax-dictionary.txt"
#define AI_TABLE_CAPACITY 750
#define MAX_SD_ENTRY_LEN 150


#define error_v(...) do {			\
	SET_ERR_V(__VA_ARGS__);			\
	goto fail;				\
} while(0)

#define error(x) do {				\
	SET_ERR(x);				\
	goto fail;				\
} while(0)


#ifndef EXCLUDE_SYNTAX_DICTIONARY_LOADER

static int processComponent(gs1_encoder* const ctx, char* const component, struct aiComponent* const part) {

	const char *token, *p;
	char *saveptr = NULL;
	int numlinters, linter;
	size_t len;

	// First component is the format specification
	token = strtok_r(component, ",", &saveptr);
	assert(token);	// Callers guarantee non-empty component
	p = token;
	len = strlen(p);
	part->opt = (*p == '[') ? OPT : MAN;		// Component is optional
	if (part->opt == OPT) {		// Find the ending ']'
		if (*(p+len-1) != ']')
			error_v(FORMAT_SPEC_FOR_OPT_COMPONENT_MISSING_RT_SQ_BRACKET, token);
		p++;
		len -= 2;	// Length does not include closing ']'
	}
	switch (*p) {
		case 'X': part->cset = cset_X;    break;
		case 'N': part->cset = cset_N;    break;
		case 'Y': part->cset = cset_Y;    break;
		case 'Z': part->cset = cset_Z;    break;
		case '_': part->cset = cset_none; break;	// Filler
		default:  error_v(UNKNOWN_CHARACTER_SET, *p);
	}

	if (len < 2)
		error_v(FORMAT_SPEC_TOO_SHORT, token);

	p++; len--;

	if ((*p >= '1' && *p <= '9') ||
	    (part->cset == cset_none && *p == '0')) {		// e.g. X12 or [X12] or "_0"

		if (len > 2) {
			error_v(AI_LENGTH_TOO_LONG, token);
		} else if (len == 2) {
			if (*(p+1) < '0' || *(p+1) > '9')
				error_v(AI_LENGTH_IS_NOT_A_NUMBER, token);
			part->min = part->max = (uint8_t)((*p - '0') * 10 + (*(p+1) - '0'));
		} else {			/* len == 1 */
			part->min = part->max = (uint8_t)(*p - '0');
		}

	} else if (len >= 3 && *p == '.' && *(p+1) == '.' &&
		   *(p+2) >= '1' && *(p+2) <= '9' ) {		// e.g. X..20 or [X..20]

		p += 2; len -= 2;

		part->min = 1;
		if (len > 2) {
			error_v(AI_LENGTH_TOO_LONG, token);
		} else if (len == 2) {
			if (*(p+1) < '0' || *(p+1) > '9')
				error_v(AI_LENGTH_IS_NOT_A_NUMBER, token);
			part->max = (uint8_t)((*p - '0') * 10 + (*(p+1) - '0'));
		} else {			/* len == 1 */
			part->max = (uint8_t)(*p - '0');
		}

	} else {
		error_v(UNRECOGNISED_FORMAT_SPECIFICATION, token);
	}

	// Remaining tokens are the names of linters
	numlinters = 0;
	while ((token = strtok_r(NULL, ",", &saveptr)) != NULL) {

		if (numlinters >= MAX_LINTERS - 1)
			error_v(NUMBER_OF_LINTERS_EXCEEDS_IMPL_LIMIT, component);

		if ((part->linters[numlinters] = gs1_linter_from_name(token)) == NULL)
			error_v(UNKNOWN_LINTER, token);

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
	char *p;
	size_t len;
	char rangeEnd;
	int numparts, part, linter;
	char buf[MAX_AI_ATTR_LEN + 2] = { 0 };
	char linebuf[MAX_SD_ENTRY_LEN + 1] = { 0 };

	*(*entry)->ai = '\0';
	(*entry)->ailen = 0;
	(*entry)->attrs = NULL;
	(*entry)->title = NULL;
	(*entry)->fnc1 = DO_FNC1;
	(*entry)->dlDataAttr = NO_DATA_ATTR;

	len = strlen(line);
	if (len > MAX_SD_ENTRY_LEN)
		error(ENTRY_TOO_LONG);

	// Do nothing with empty and comment-only lines
	memcpy(linebuf, line, len + 1);			// Includes NULL
	token = strtok_r(linebuf, " \t", &saveptr);
	if (!token || *token == '#')
		return 0;

	if ((uint16_t)(*entry - sd) >= cap - 1)
		error(SYNTAX_DICTIONARY_CAPACITY_TOO_SMALL);

	// Initial token should be an AI or an AI range
	len = strlen(token);
	if ((p = strchr(token, '-')) != NULL) {

		if (len < MIN_AI_LEN*2+1 || len > MAX_AI_LEN*2+1)
			error(AI_RANGE_HAS_WRONG_WIDTH);

		if ((len%2 != 1) || ((size_t)(p - token) != len/2))
			error(AIS_IN_RANGE_MUST_HAVE_EQUAL_WIDTH);

		if (strspn(token, "0123456789") != len/2 ||
		    strspn(token + len/2+1, "0123456789") != len/2)
			error(AIS_MUST_BE_NUMERIC);

		if (memcmp(token, token + len/2+1, len/2-1) != 0)
			error(AI_RANGE_PARTS_MAY_ONLY_DIFFER_IN_LAST_DIGIT);

		if (*(token + len/2-1) >= *(token + len-1))
			error(AI_RANGE_END_MUST_EXCEED_RANGE_START);

		memcpy((*entry)->ai, token, len/2);
		(*entry)->ai[len/2] = '\0';
		(*entry)->ailen = (uint8_t)len/2;

		rangeEnd = *(token + len-1);
		len /= 2;

	} else {

		if (len < MIN_AI_LEN || len > MAX_AI_LEN)
			error(AI_HAS_WRONG_WIDTH);

		if (strspn(token, "0123456789") != len)
			error(AI_MUST_BE_NUMERIC);

		memcpy((*entry)->ai, token, len);
		(*entry)->ai[len] = '\0';
		(*entry)->ailen = (uint8_t)len;

		rangeEnd = *(token + len-1);

	}

	token = strtok_r(NULL, " \t", &saveptr);
	if (!token)
		error(TRUNCATED_AFTER_AI);

	/*
	 *  There may be multiple whilespace-separated, tokenised flags. If the
	 *  token has exclusively flag characters then process as such.
	 *
	 */
	while (token[strspn(token, "*?!\"$%&'()+,-./:;<=>@[\\]^_`{|}~")] == '\0') {

		// '*' => no FNC1 indicator flag
		if (strchr(token, '*'))
			(*entry)->fnc1 = NO_FNC1;

		// '!' => designated GS1 identification key indicator flag
		// Note: There is no action for this at the moment

		// '?' => permit GS1 DL URI data attribute indicator flag
		if (strchr(token, '?'))
			(*entry)->dlDataAttr = DL_DATA_ATTR;

		token = strtok_r(NULL, " \t", &saveptr);
		if (!token)
			error(TRUNCATED_AFTER_FLAGS);

	}

	// Read and process the AI components
	numparts = 0;
	while (token && ((*token >= 'A' && *token <= 'Z') || *token == '[')) {
		if (numparts >= MAX_PARTS - 1)
			error(NUMBER_OF_AI_COMPONENTS_EXCEEDS_IMPL);

		if (processComponent(ctx, (char*)token, &(*entry)->parts[numparts]) < 0)
			goto fail;

		numparts++;
		token = strtok_r(NULL, " \t", &saveptr);
	}
	if (numparts == 0)
		error(AI_IS_MISSING_COMPONENTS);

	// Sanity checks over the components to avoid specifications that are ambiguous
	for (part = 0; part < MAX_PARTS; part++) {
		struct aiComponent* const c = &(*entry)->parts[part];
		if (part >= numparts) {		// Fillers for parts
			processComponent(ctx, "_0", c);
			continue;
		}
		if (part < numparts-1 && c->min != c->max)
			error(ONLY_FINAL_COMPONENT_MAY_HAVE_VARIABLE_LENGTH);
		if (part > 0 && c->opt == MAN && (c-1)->opt == OPT)
			error(MANDATORY_COMPONENT_CANNOT_FOLLOW_OPTIONAL_COMPONENTS);
	}

	// Read the key/value attributes until the title delimiter
	p = buf;
	while (token && strcmp(token, "#") != 0) {

		char *q;
		size_t toklen;

		if ((q = strchr(token, '=')) != NULL) {		// e.g. dlpkey=1,2,3

			if (token == q)
				error(ATTRIBUTE_NAME_REQUIRED_ON_LHS_OF_ASSIGNMENT);

			*q = '\0';
			if (token[strspn(token, "abcdefghijklmnopqrstuvwxyz")] != '\0')
				error(ATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS);
			*q = '=';

			if ((q+1)[strspn(q+1, "abcdefghijklmnopqrstuvwxyz0123456789-+_,|")] != '\0')
				error(ATTRIBUTE_VALUE_CONTAINS_ILLEGAL_CHARACTERS);

			if (*(q+1) == '\0')
				error(ATTRIBUTE_VALUE_REQUIRED_ON_RHS_OF_ASSIGNMENT);

		} else {					// e.g. dlpkey

			if (token[strspn(token, "abcdefghijklmnopqrstuvwxyz")] != '\0')
				error(SINGLETON_ATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS);

		}

		toklen = strlen(token);
		if (toklen >= sizeof(buf) - (size_t)(p-buf) - 1)
			error(ATTRIBUTES_TOO_LONG);
		memcpy(p, token, toklen);
		p += toklen;
		*p++ = ' ';

		token = strtok_r(NULL, " \t", &saveptr);

	}
	if (p != buf)
		*(p-1) = '\0';			// Chop final space
	(*entry)->attrs = gs1_strdup_alloc(buf);
	if (!(*entry)->attrs)
		error(FAILED_TO_ALLOCATE_MEMORY_FOR_ATTRS);

	// Read until the end of line for the title
	token = strtok_r(NULL, "", &saveptr);
	if (token) {

		if (token[strspn(token, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890#()-+,./²³ ")] != '\0')
			error(TITLE_CONTAINS_ILLEGAL_CHARACTERS);

		(*entry)->title = gs1_strdup_alloc(token);
		if (!(*entry)->title)
			error(FAILED_TO_ALLOCATE_MEMORY_FOR_TITLE);

	} else {
		(*entry)->title = gs1_strdup_alloc("");
		if (!(*entry)->title)
			error(FAILED_TO_ALLOCATE_MEMORY_FOR_TITLE);
	}

	// Duplicate the initial entry to fill down to the end of the range
	lastEntry = (*entry)++;
	while (lastEntry->ai[len-1] != rangeEnd) {

		if ((uint16_t)(*entry - sd) >= cap - 1)
			error(SYNTAX_DICTIONARY_CAPACITY_TOO_SMALL);

		memcpy((*entry)->ai, lastEntry->ai, lastEntry->ailen + 1);	// Includes NULL
		(*entry)->ai[len-1]++;
		(*entry)->ailen = lastEntry->ailen;
		(*entry)->fnc1 = lastEntry->fnc1;
		(*entry)->dlDataAttr = lastEntry->dlDataAttr;
		for (part = 0; part < MAX_PARTS; part++) {
			(*entry)->parts[part].cset = lastEntry->parts[part].cset;
			(*entry)->parts[part].opt  = lastEntry->parts[part].opt;
			(*entry)->parts[part].min  = lastEntry->parts[part].min;
			(*entry)->parts[part].max  = lastEntry->parts[part].max;
			for (linter = 0; linter < MAX_LINTERS; linter++)
				(*entry)->parts[part].linters[linter] = lastEntry->parts[part].linters[linter];
		}
		(*entry)->attrs = gs1_strdup_alloc(lastEntry->attrs);
		if (!(*entry)->attrs)
			error(FAILED_TO_ALLOCATE_MEMORY_FOR_ATTRS);
		(*entry)->title = gs1_strdup_alloc(lastEntry->title);
		if (!(*entry)->title)
			error(FAILED_TO_ALLOCATE_MEMORY_FOR_TITLE);

		(*entry)++;
		lastEntry++;

	}

	// Prepare the next entry
	*(*entry)->ai = '\0';

	return (int16_t)( (*entry) - sd);

fail:
	*(*entry)->ai = '\0';
	GS1_ENCODERS_FREE((*entry)->title);
	(*entry)->title = NULL;
	GS1_ENCODERS_FREE((*entry)->attrs);
	(*entry)->attrs = NULL;
	return -1;

}

static struct aiEntry* parseSyntaxDictionaryFile(gs1_encoder* const ctx, const char* const fname) {

	const uint16_t cap = AI_TABLE_CAPACITY;
	FILE *fp = NULL;
	char buf[MAX_SD_ENTRY_LEN + 2];		// fgets includes "\n\0"
	size_t linenum;

	struct aiEntry *sd;
	struct aiEntry *pos;

	sd = (struct aiEntry*)GS1_ENCODERS_MALLOC(cap * sizeof(struct aiEntry));
	if (!sd)
		error(FAILED_TO_ALLOCATE_AI_TABLE);
	sd[0].ai[0] = '\0';
	sd[0].attrs = NULL;
	sd[0].title = NULL;

	fp = fopen(fname, "r");
	if (fp == NULL)
		error_v(CANNOT_READ_FILE, fname);

	pos = sd;
	linenum = 1;
	while (fgets(buf, sizeof(buf), fp)) {
		if (buf[strlen(buf)-1] != '\n' && !feof(fp))
			error_v(SYNTAX_DICTIONARY_LINE_EXCEEDS_IMPL, (int)linenum, MAX_SD_ENTRY_LEN);
		buf[strcspn(buf, "\r\n")] = 0;		/* Chop linefeed and newline */
		if (parseSyntaxDictionaryEntry(ctx, buf, sd, &pos, cap) < 0) {
			char errbuf[sizeof(ctx->errMsg)];
			size_t len;
			memcpy(errbuf, ctx->errMsg, sizeof(errbuf));
			len = strlen(errbuf);
			if (len > sizeof(ctx->errMsg) - 50) len = sizeof(ctx->errMsg) - 50;
			error_v(SYNTAX_DICTIONARY_LINE_ERROR, (int)linenum, (int)len, errbuf);
		}
		linenum++;
	}

	fclose(fp);

	return sd;

fail:
	if (fp) fclose(fp);
	if (sd) gs1_freeSyntaxDictionaryEntries(ctx, sd);
	GS1_ENCODERS_FREE(sd);
	return NULL;

}

#undef error
#undef error_v


struct aiEntry* gs1_loadSyntaxDictionary(gs1_encoder* const ctx, const char *fname, bool quiet) {

	struct aiEntry *sd;

	const char* const filename = fname ? fname : DEFAULT_SYNTAX_FILENAME;

	/*
	 * If a name isn't provided then attempt to load the default Syntax Dictionary file.
	 *
	 */
	if ((sd = parseSyntaxDictionaryFile(ctx, filename)) == NULL) {
		if (!quiet) {
			printf("\n*** Failed to parse Syntax Dictionary file: %s\n", filename);
			printf("*** %s\n", ctx->errMsg);
		}
	}

	return sd;

}

void gs1_freeSyntaxDictionaryEntries(const gs1_encoder* const ctx, struct aiEntry *sd) {

	(void)ctx;

	assert(sd);

	while (*sd->ai) {
		*sd->ai = '\0';
		GS1_ENCODERS_FREE(sd->attrs);
		sd->attrs = NULL;
		GS1_ENCODERS_FREE(sd->title);
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
	char buf[256] = {0};
	struct aiEntry *out, *tmp;

	TEST_CASE(sdEntry);

	TEST_ASSERT((out = GS1_ENCODERS_CALLOC(cap, sizeof(struct aiEntry))) != NULL);
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
		TEST_CHECK(out[i].dlDataAttr == expectedAIentries[i].dlDataAttr);
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
	GS1_ENCODERS_FREE(out);

}


struct test_parse_sd_entry_s {
	bool expectSuccess;
	char* sdEntry;
	struct aiEntry aiEntries[11];
};

struct test_parse_sd_entry_s tests_parse_sd_entry[] = {

	/*
	 *  Empty lines and comments
	 *
	 */
	{ true,  "",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "#",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# ",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# COMMENT",			{ AI_ENTRY_TERMINATOR } },

	/*
	 *  AI: single AI, various widths
	 *
	 */
	{ true, "90  ?  X..30  # INTERNAL", {						/* Single AI */
		AI_ENTRY("90", DO_FNC1, DL_DATA_ATTR, X,1,30,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "253  ?  N13,csum,gcppos1 [X..17]  dlpkey  # GDTI", {			/* 3-digit AI, optional component */
		AI_ENTRY("253", DO_FNC1, DL_DATA_ATTR, N,13,13,MAN,csum,gcppos1,_, X,1,17,OPT,_,_,_, __, __, __, "dlpkey", "GDTI"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8200  X..70  req=01  # PRODUCT URL", {				/* 4-digit AI, not GS1 DL URI data attr */
		AI_ENTRY("8200", DO_FNC1, NO_DATA_ATTR, X,1,70,MAN,_,_,_, __, __, __, __, "req=01", "PRODUCT URL"),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "9  X1", {								/* AI too short */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "12345  X1", {								/* AI too long */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "9a  X1", {								/* AI not numeric */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  AI: range of AIs
	 *
	 */
	{ true, "91-99  ?  X..90  # INTERNAL", {					/* Range of AIs */
		AI_ENTRY("91", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("92", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("93", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("94", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("95", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("96", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("97", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("98", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY("99", DO_FNC1, DL_DATA_ATTR, X,1,90,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "9-99  X1", {								/* Range: wrong width */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "9-099  X1", {								/* Range: unequal width */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "9a-99  X1", {								/* Range: non-numeric */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "10-29  X1", {								/* Range: differ in more than last digit */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "95-91  X1", {								/* Range: end does not exceed start */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Flags: *, !, ?
	 *
	 */
	{ true, "01  * ?  N14,csum,gcppos1  ex=02,255,37  dlpkey=22,10,21|235  # GTIN", {	/* FNC1 not required, gappy flags */
		AI_ENTRY("01", NO_FNC1, DL_DATA_ATTR, N,14,14,MAN,csum,gcppos1,_, __, __, __, __, "ex=02,255,37 dlpkey=22,10,21|235", "GTIN"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "414  *!?  N13,csum,gcppos1  dlpkey=254|7040  # LOC No.", {		/* Christmas tree case for flags */
		AI_ENTRY("414", NO_FNC1, DL_DATA_ATTR, N,13,13,MAN,csum,gcppos1,_, __, __, __, __, "dlpkey=254|7040", "LOC No."),
	} },
	{ false, "90", {								/* Truncated after AI */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?", {								/* Truncated after flags */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: character sets (X, N, Y, Z)
	 *
	 */
	{ true, "8010  ?  Y..30,gcppos1  dlpkey=8011  # CPID", {			/* CSET 32 */
		AI_ENTRY("8010", DO_FNC1, DL_DATA_ATTR, Y,1,30,MAN,gcppos1,_,_, __, __, __, __, "dlpkey=8011", "CPID"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4307  ?  Z2,iso3166alpha2  # COUNTRY - INITIAL PROCESS.", {		/* CSET Z */
		AI_ENTRY("4307", DO_FNC1, DL_DATA_ATTR, Z,2,2,MAN,iso3166alpha2,_,_, __, __, __, __, "", "COUNTRY - INITIAL PROCESS."),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  Q5", {								/* Unknown character set */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: fixed-length format spec
	 *
	 */
	{ false, "90  X", {								/* Format spec too short */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X0", {								/* Unrecognised format spec */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X123", {								/* Fixed-length: too many digits */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X1a", {								/* Fixed-length: non-digit in length */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: variable-length format spec
	 *
	 */
	{ false, "90  X..123", {							/* Variable-length: too many digits */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X..1a", {								/* Variable-length: non-digit in max */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: optional components
	 *
	 */
	{ true, "7007  ?  N6,yymmdd [N..6],yymmdd  req=01,02  # HARVEST DATE", {	/* Requisites */
		AI_ENTRY("7007", DO_FNC1, DL_DATA_ATTR, N,6,6,MAN,yymmdd,_,_, N,1,6,MAN,yymmdd,_,_, __, __, __, "req=01,02", "HARVEST DATE"),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  [N5", {								/* Optional missing closing bracket */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?  [N5] X5", {							/* Mandatory component follows optional */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: count and ordering
	 *
	 */
	{ true, "8001  ?  N4,nonzero N5,nonzero N3,nonzero N1,winding N1  req=01  # DIMENSIONS", {	/* Max components */
		AI_ENTRY("8001", DO_FNC1, DL_DATA_ATTR, N,4,4,MAN,nonzero,_,_, N,5,5,MAN,nonzero,_,_, N,3,3,MAN,nonzero,_,_, N,1,1,MAN,winding,_,_, N,1,1,MAN,_,_,_, "req=01", "DIMENSIONS"),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  #", {								/* No components */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?  N1 N1 N1 N1 N1 N1", {						/* Too many components (6 > max 5) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?  N..5 X..30", {							/* Non-final, variable-length component */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Components: linters
	 *
	 */
	{ true, "8014  X..25,csumalpha,gcppos1,hasnondigit  req=01  # MUDI", {		/* Max linters */
		AI_ENTRY("8014", DO_FNC1, NO_DATA_ATTR, X,1,25,MAN,csumalpha,gcppos1,hasnondigit, __, __, __, __, "req=01", "MUDI"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4308  ?  N3,iso3166 N3,iso4217 N1,iso5218  # COUNTRY/CURRENCY", {
		AI_ENTRY("4308", DO_FNC1, DL_DATA_ATTR, N,3,3,MAN,iso3166,_,_, N,3,3,MAN,iso4217,_,_, N,1,1,MAN,iso5218,_,_, __, __, "", "COUNTRY/CURRENCY"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4309  ?  N3,iso3166999 X..34,iban  # EXTENDED COUNTRY", {
		AI_ENTRY("4309", DO_FNC1, DL_DATA_ATTR, N,3,3,MAN,iso3166999,_,_, X,1,34,MAN,iban,_,_, __, __, __, "", "EXTENDED COUNTRY"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "7003  ?  N8,yyyymmdd N2,hh N2,mi [N2],ss  # EXPIRATION DATE/TIME", {
		AI_ENTRY("7003", DO_FNC1, DL_DATA_ATTR, N,8,8,MAN,yyyymmdd,_,_, N,2,2,MAN,hh,_,_, N,2,2,MAN,mi,_,_, N,2,2,OPT,ss,_,_, __, "", "EXPIRATION DATE/TIME"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "7006  ?  N6,yymmd0 [N8],yyyymmd0  # FIRST FREEZE DATE", {
		AI_ENTRY("7006", DO_FNC1, DL_DATA_ATTR, N,6,6,MAN,yymmd0,_,_, N,8,8,OPT,yyyymmd0,_,_, __, __, __, "", "FIRST FREEZE DATE"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4310  ?  N4,hhmi N10,latitude N11,longitude  # GEO/TIME", {
		AI_ENTRY("4310", DO_FNC1, DL_DATA_ATTR, N,4,4,MAN,hhmi,_,_, N,10,10,MAN,latitude,_,_, N,11,11,MAN,longitude,_,_, __, __, "", "GEO/TIME"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4320  ?  X20,cset82,cset39,cset64  # CSET TEST", {
		AI_ENTRY("4320", DO_FNC1, DL_DATA_ATTR, X,20,20,MAN,cset82,cset39,cset64, __, __, __, __, "", "CSET TEST"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4321  ?  N10,csetnumeric X..10,hyphen  # CSET TEST 2", {
		AI_ENTRY("4321", DO_FNC1, DL_DATA_ATTR, N,10,10,MAN,csetnumeric,_,_, X,1,10,MAN,hyphen,_,_, __, __, __, "", "CSET TEST 2"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4322  ?  N1,yesno N1,zero [N..5],nozeroprefix  # FLAGS", {
		AI_ENTRY("4322", DO_FNC1, DL_DATA_ATTR, N,1,1,MAN,yesno,_,_, N,1,1,MAN,zero,_,_, N,1,5,OPT,nozeroprefix,_,_, __, __, "", "FLAGS"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8112  ?  X..70,couponposoffer  # PAPERLESS COUPON", {
		AI_ENTRY("8112", DO_FNC1, DL_DATA_ATTR, X,1,70,MAN,couponposoffer,_,_, __, __, __, __, "", "PAPERLESS COUPON"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4323  ?  X4,pieceoftotal [X..10],posinseqslash  # PIECE", {
		AI_ENTRY("4323", DO_FNC1, DL_DATA_ATTR, X,4,4,MAN,pieceoftotal,_,_, X,1,10,OPT,posinseqslash,_,_, __, __, __, "", "PIECE"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4324  ?  X..70,pcenc  # ENCODED DATA", {
		AI_ENTRY("4324", DO_FNC1, DL_DATA_ATTR, X,1,70,MAN,pcenc,_,_, __, __, __, __, "", "ENCODED DATA"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4325  ?  X..3,packagetype  # PACKAGE TYPE", {
		AI_ENTRY("4325", DO_FNC1, DL_DATA_ATTR, X,1,3,MAN,packagetype,_,_, __, __, __, __, "", "PACKAGE TYPE"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4326  ?  X3,importeridx [X..70],mediatype  # IMPORT MEDIA", {
		AI_ENTRY("4326", DO_FNC1, DL_DATA_ATTR, X,3,3,MAN,importeridx,_,_, X,1,70,OPT,mediatype,_,_, __, __, __, "", "IMPORT MEDIA"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "4329  ?  X20,gcppos2 [N..2],ss  # CODE", {
		AI_ENTRY("4329", DO_FNC1, DL_DATA_ATTR, X,20,20,MAN,gcppos2,_,_, N,1,2,OPT,ss,_,_, __, __, __, "", "CODE"),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5,csum,csum,csum,csum", {					/* Too many linters (4 > max 3) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5,badlinter", {							/* Unknown linter */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Attributes
	 *
	 */
	{ true, "90  ?  X..30  req=999", {						/* Attrs and no title */
		AI_ENTRY("90", DO_FNC1, DL_DATA_ATTR, X,1,30,MAN,_,_,_, __, __, __, __, "req=999", ""),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  =value", {							/* Missing attribute name (LHS) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  Req=01", {							/* Attribute name: illegal chars (caught as bad component) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  req1=01", {							/* Attribute name: illegal chars (digit in name) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  req=!!", {							/* Attribute value: illegal chars */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  req=", {							/* Missing attribute value (RHS) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  Req", {							/* Singleton name: illegal chars (caught as bad component) */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  req1", {							/* Singleton name: illegal chars (digit in name) */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Title
	 *
	 */
	{ true, "8110  ?  X..70,couponcode", {						/* No attrs and no title */
		AI_ENTRY("8110", DO_FNC1, DL_DATA_ATTR, X,1,70,MAN,couponcode,_,_, __, __, __, __, "", ""),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  X5  # @title", {							/* Title: illegal characters */
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Attributes too long: cumulative attributes exceed MAX_AI_ATTR_LEN + 2 buffer
	 *
	 */
	{ false, "90  X5  req=aaaaaaaaaa req=bbbbbbbbbb req=cccccccccc req=dddddddddd req=eeeeeeeeee", {
		AI_ENTRY_TERMINATOR
	} },

	/*
	 *  Entry too long: exceeds MAX_SD_ENTRY_LEN (150)
	 *
	 */
	{ false, "90  X5  # title_padding_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", {
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


#endif  /* EXCLUDE_SYNTAX_DICTIONARY_LOADER */
