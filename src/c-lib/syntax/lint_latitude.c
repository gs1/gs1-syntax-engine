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
 * @file lint_latitude.c
 *
 * @brief The `latitude` linter ensures that the given data represents the
 * latitude component of a WGS84 coordinate expressed as a 10-digit number.
 *
 */

#include <assert.h>
#include <stdlib.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component represents the latitude component of a
 * WGS84 coordinate expressed as a 10-digit number.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_LATITUDE_INVALID_LENGTH if the latitude does not have length 10.
 * @return #GS1_LINTER_INVALID_LATITUDE if the latitude is outside the range `0000000000` - `1800000000`.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_latitude(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;
	unsigned long value;

	assert(data);

	/*
	 * Data must be 10 characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len != 10))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_LATITUDE_INVALID_LENGTH,
			0,
			data_len
		);

	/*
	 * Data must consist of all digits and latitude must be within
	 * 0000000000 to 1800000000.
	 *
	 */
	for (pos = 0, value = 0; pos < data_len; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);
		value = value * 10 + (unsigned long)(data[pos] - '0');
	}

	if (GS1_LINTER_UNLIKELY(value > 1800000000))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INVALID_LATITUDE,
			0,
			10
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_latitude(void)
{

	UNIT_TEST_PASS(gs1_lint_latitude, "0279085848");
	UNIT_TEST_PASS(gs1_lint_latitude, "0000000000");
	UNIT_TEST_PASS(gs1_lint_latitude, "1800000000");

	UNIT_TEST_FAIL(gs1_lint_latitude, "027908584", GS1_LINTER_LATITUDE_INVALID_LENGTH, "*027908584*");
	UNIT_TEST_FAIL(gs1_lint_latitude, "02790858481", GS1_LINTER_LATITUDE_INVALID_LENGTH, "*02790858481*");

	UNIT_TEST_FAIL(gs1_lint_latitude, "1800000001", GS1_LINTER_INVALID_LATITUDE, "*1800000001*");

}

#endif  /* UNIT_TESTS */
