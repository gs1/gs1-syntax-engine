/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_longitude.c
 *
 * @brief The `longitude` linter ensures that the given data represents the
 * longitude component of a WGS84 coordinate expressed as a 10-digit number.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component represents the longitude component of a
 * WGS84 coordinate expressed as a 10-digit number.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_LONGITUDE_INVALID_LENGTH if the longitude does not have length 10.
 * @return #GS1_LINTER_INVALID_LONGITUDE if the longitude is outside the range `0000000000` - `3600000000`.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_longitude(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);

	/*
	 * Data must consist of all digits.
	 *
	 */
	for (pos = 0; pos < 10 && data[pos]; pos++)
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);

	/*
	 * Data must be 10 characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(pos != 10 || data[10]))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_LONGITUDE_INVALID_LENGTH,
			0,
			pos + (data[pos] ? 1 : 0)
		);

	/*
	 * The longitude must be within the range 0000000000 to 3600000000.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(strtoul(data, NULL, 10) > 3600000000))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INVALID_LONGITUDE,
			0,
			10
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_longitude(void)
{

	UNIT_TEST_PASS(gs1_lint_longitude, "3015297971");
	UNIT_TEST_PASS(gs1_lint_longitude, "0000000000");
	UNIT_TEST_PASS(gs1_lint_longitude, "3600000000");

	UNIT_TEST_FAIL(gs1_lint_longitude, "301529797", GS1_LINTER_LONGITUDE_INVALID_LENGTH, "*301529797*");
	UNIT_TEST_FAIL(gs1_lint_longitude, "30152979711", GS1_LINTER_LONGITUDE_INVALID_LENGTH, "*30152979711*");

	UNIT_TEST_FAIL(gs1_lint_longitude, "3600000001", GS1_LINTER_INVALID_LONGITUDE, "*3600000001*");

}

#endif  /* UNIT_TESTS */
