/*
 * GS1 Syntax Dictionary. Copyright (c) 2022 GS1 AISBL.
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

/**
 * @file lint_iso3166list.c
 *
 * @brief The `iso3166list` linter ensures that the data is a concatenated
 * sequence of ISO 3166 "num-3" country codes.
 *
 * @remark The three-digit country codes are defined by [ISO 3166-1: Codes for the representation of names of countries and their subdivisions - Part 1: Country code](https://www.iso.org/standard/72482.html) as the "num-3" codes.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to validate that an AI component is a concatenated sequence of ISO 3166
 * "num-3" country codes.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NOT_ISO3166 if the data contains a part of the sequence
 *         that is not a num-3 country code, or the data is empty.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso3166list(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	const char *p, *q;
	char c3[4] = {0};
	gs1_lint_err_t ret;

	assert(data);

	p = data;
	q = p + strlen(data);

	/*
	 *  Process each group of three characters to check if it is a valid
	 *  ISO 3166 num-3 country code.
	 *
	 */
	while (p <= q - 3) {

		strncpy(c3, p, 3);

		ret = gs1_lint_iso3166(c3, err_pos, err_len);
		assert(ret == GS1_LINTER_OK || ret == GS1_LINTER_NOT_ISO3166);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 3;
			return ret;
		}

		p += 3;

	}

	/*
	 *  Any remaining characters or an empty list are invalid.
	 *
	 */
	if (p != q || q - data == 0) {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = (size_t)(q - p);
		return GS1_LINTER_NOT_ISO3166;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_iso3166list(void)
{

	UNIT_TEST_PASS(gs1_lint_iso3166list, "004");
	UNIT_TEST_PASS(gs1_lint_iso3166list, "894");
	UNIT_TEST_PASS(gs1_lint_iso3166list, "004894");
	UNIT_TEST_PASS(gs1_lint_iso3166list, "004270894");

	UNIT_TEST_FAIL(gs1_lint_iso3166list, "", GS1_LINTER_NOT_ISO3166, "**");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "0", GS1_LINTER_NOT_ISO3166, "*0*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "00", GS1_LINTER_NOT_ISO3166, "*00*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "000", GS1_LINTER_NOT_ISO3166, "*000*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "0000", GS1_LINTER_NOT_ISO3166, "*000*0");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "00000", GS1_LINTER_NOT_ISO3166, "*000*00");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "000000", GS1_LINTER_NOT_ISO3166, "*000*000");

	UNIT_TEST_FAIL(gs1_lint_iso3166list, "8940", GS1_LINTER_NOT_ISO3166, "894*0*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "89400", GS1_LINTER_NOT_ISO3166, "894*00*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "894000", GS1_LINTER_NOT_ISO3166, "894*000*");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "8940000", GS1_LINTER_NOT_ISO3166, "894*000*0");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "89400000", GS1_LINTER_NOT_ISO3166, "894*000*00");
	UNIT_TEST_FAIL(gs1_lint_iso3166list, "894000000", GS1_LINTER_NOT_ISO3166, "894*000*000");

}

#endif  /* UNIT_TESTS */
