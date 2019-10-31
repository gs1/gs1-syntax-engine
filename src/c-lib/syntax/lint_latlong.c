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
 * @file lint_latlong.c
 *
 * @brief The `latlong` linter ensures that the given data represents a WGS84
 * coordinate expressed as the concatenation of two 10-digit numbers.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to validate that an AI component represents a WGS84 coordinate
 * expressed as the concatenation of two 10-digit numbers.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_LATLONG_INVALID_LENGTH if the concatenated latitude and longitude does not have length 20.
 * @return #GS1_LINTER_INVALID_LATITUDE if the latitude part is outside the range `0000000000` - `1800000000`.
 * @return #GS1_LINTER_INVALID_LONGITUDE if the latitude part is outside the range `0000000000` - `3600000000`.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_latlong(const char *data, size_t *err_pos, size_t *err_len)
{

	char part[11] = {0};
	size_t len, pos;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be 20 characters.
	 *
	 */
	if (len != 20) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_LATLONG_INVALID_LENGTH;
	}

	/*
	 * Data must consist of all digits.
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != len) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_NON_DIGIT_CHARACTER;
	}

	/*
	 * The latitude component data must be within the range 0000000000 to
	 * 1800000000.
	 *
	 */
	memcpy(part, data, 10);
	if (strtoul(part, NULL, 10) > 1800000000) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 10;
		return GS1_LINTER_INVALID_LATITUDE;
	}

	/*
	 * The longitude component data must be within the range 0000000000 to
	 * 3600000000.
	 *
	 */
	memcpy(part, data+10, 10);
	if (strtoul(part, NULL, 10) > 3600000000) {
		if (err_pos) *err_pos = 10;
		if (err_len) *err_len = 10;
		return GS1_LINTER_INVALID_LONGITUDE;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_latlong(void)
{

	UNIT_TEST_PASS(gs1_lint_latlong, "02790858483015297971");
	UNIT_TEST_PASS(gs1_lint_latlong, "00000000000000000000");
	UNIT_TEST_PASS(gs1_lint_latlong, "18000000003600000000");

	UNIT_TEST_FAIL(gs1_lint_latlong, "0279085848301529797", GS1_LINTER_LATLONG_INVALID_LENGTH, "*0279085848301529797*");
	UNIT_TEST_FAIL(gs1_lint_latlong, "027908584830152979711", GS1_LINTER_LATLONG_INVALID_LENGTH, "*027908584830152979711*");

	UNIT_TEST_FAIL(gs1_lint_latlong, "18000000010000000000", GS1_LINTER_INVALID_LATITUDE, "*1800000001*0000000000");
	UNIT_TEST_FAIL(gs1_lint_latlong, "00000000003600000001", GS1_LINTER_INVALID_LONGITUDE, "0000000000*3600000001*");
	UNIT_TEST_FAIL(gs1_lint_latlong, "18000000013600000001", GS1_LINTER_INVALID_LATITUDE, "*1800000001*3600000001");

}

#endif  /* UNIT_TESTS */
