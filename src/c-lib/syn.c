/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2021-2025 GS1 AISBL.
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

	*(*entry)->ai = '\0';
	(*entry)->ailen = 0;
	(*entry)->attrs = NULL;
	(*entry)->title = NULL;
	(*entry)->fnc1 = DO_FNC1;
	(*entry)->dlDataAttr = NO_DATA_ATTR;

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
		size_t len;

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

		len = strlen(token);
		if (len >= sizeof(buf) - (size_t)(p-buf) - 1)
			error(ATTRIBUTES_TOO_LONG);
		memcpy(p, token, len);
		p += len;
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
	gs1_freeSyntaxDictionaryEntries(ctx, sd);
	GS1_ENCODERS_FREE(sd);
	return NULL;

}

#undef error
#undef error_v


struct aiEntry* gs1_loadSyntaxDictionary(gs1_encoder* const ctx, const char *fname) {

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
	char buf[256];
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
	{ true,  "",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "#",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# ",				{ AI_ENTRY_TERMINATOR } },
	{ true,  "# COMMENT",			{ AI_ENTRY_TERMINATOR } },
	{ true, "90  ?  X..30  # INTERNAL", {						/* Single AI */
		AI_ENTRY("90", DO_FNC1, DL_DATA_ATTR, X,1,30,MAN,_,_,_, __, __, __, __, "", "INTERNAL"),
		AI_ENTRY_TERMINATOR
	} },
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
	{ true, "8010  ?  Y..30,gcppos1  dlpkey=8011  # CPID", {			/* CSET 32 */
		AI_ENTRY("8010", DO_FNC1, DL_DATA_ATTR, Y,1,30,MAN,gcppos1,_,_, __, __, __, __, "dlpkey=8011", "CPID"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "253  ?  N13,csum,gcppos1 [X..17]  dlpkey  # GDTI", {			/* Optional component */
		AI_ENTRY("253", DO_FNC1, DL_DATA_ATTR, N,13,13,MAN,csum,gcppos1,_, X,1,17,OPT,_,_,_, __, __, __, "dlpkey", "GDTI"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "7007  ?  N6,yymmdd [N..6],yymmdd  req=01,02  # HARVEST DATE", {	/* Requisites */
		AI_ENTRY("7007", DO_FNC1, DL_DATA_ATTR, N,6,6,MAN,yymmdd,_,_, N,1,6,MAN,yymmdd,_,_, __, __, __, "req=01,02", "HARVEST DATE"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "01  * ?  N14,csum,gcppos1  ex=02,255,37  dlpkey=22,10,21|235  # GTIN", {	/* FNC1 not required, gappy flags */
		AI_ENTRY("01", NO_FNC1, DL_DATA_ATTR, N,14,14,MAN,csum,gcppos1,_, __, __, __, __, "ex=02,255,37 dlpkey=22,10,21|235", "GTIN"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "414  *!?  N13,csum,gcppos1  dlpkey=254|7040  # LOC No.", {		/* Christmas tree case for flags */
		AI_ENTRY("414", NO_FNC1, DL_DATA_ATTR, N,13,13,MAN,csum,gcppos1,_, __, __, __, __, "dlpkey=254|7040", "LOC No."),
	} },
	{ true, "8200  X..70  req=01  # PRODUCT URL", {					/* Not GS1 DL URI data attr */
		AI_ENTRY("8200", DO_FNC1, NO_DATA_ATTR, X,1,70,MAN,_,_,_, __, __, __, __, "req=01", "PRODUCT URL"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8001  ?  N4,nonzero N5,nonzero N3,nonzero N1,winding N1  req=01  # DIMENSIONS", {	/* Max components */
		AI_ENTRY("8001", DO_FNC1, DL_DATA_ATTR, N,4,4,MAN,nonzero,_,_, N,5,5,MAN,nonzero,_,_, N,3,3,MAN,nonzero,_,_, N,1,1,MAN,winding,_,_, N,1,1,MAN,_,_,_, "req=01", "DIMENSIONS"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8014  X..25,csumalpha,gcppos1,hasnondigit  req=01  # MUDI", {		/* Max linters */
		AI_ENTRY("8014", DO_FNC1, NO_DATA_ATTR, X,1,25,MAN,csumalpha,gcppos1,hasnondigit, __, __, __, __, "req=01", "MUDI"),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "8110  ?  X..70,couponcode", {						/* No attrs and no title */
		AI_ENTRY("8110", DO_FNC1, DL_DATA_ATTR, X,1,70,MAN,couponcode,_,_, __, __, __, __, "", ""),
		AI_ENTRY_TERMINATOR
	} },
	{ true, "90  ?  X..30  req=999", {						/* Bespoke test for attrs and no title */
		AI_ENTRY("90", DO_FNC1, DL_DATA_ATTR, X,1,30,MAN,_,_,_, __, __, __, __, "req=999", ""),
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?  N..5 X..30", {							/* Bespoke test for non-final, variable-length component */
		AI_ENTRY_TERMINATOR
	} },
	{ false, "90  ?  [N5] X5", {							/* Bespoke test mandatory component follows optional component */
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
