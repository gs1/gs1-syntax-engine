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
 * @file lint_hyphen.c
 *
 * @brief The `hyphen` linter ensures that the given data contains only hyphens.
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component contains only hyphens.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NOT_HYPHEN if the data contains a non-hyphen character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hyphen(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);

	if (GS1_LINTER_UNLIKELY(data_len == 0))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NOT_HYPHEN,
			0,
			0
		);

	for (pos = 0; pos < data_len; pos++)
		if (GS1_LINTER_UNLIKELY(data[pos] != '-')) {
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NOT_HYPHEN,
				0,
				data_len
			);
		}

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hyphen(void)
{

	UNIT_TEST_PASS(gs1_lint_hyphen, "-");
	UNIT_TEST_PASS(gs1_lint_hyphen, "--");

	UNIT_TEST_FAIL(gs1_lint_hyphen, "", GS1_LINTER_NOT_HYPHEN, "**");
	UNIT_TEST_FAIL(gs1_lint_hyphen, "X", GS1_LINTER_NOT_HYPHEN, "*X*");
	UNIT_TEST_FAIL(gs1_lint_hyphen, "XX", GS1_LINTER_NOT_HYPHEN, "*XX*");

}

#endif  /* UNIT_TESTS */
