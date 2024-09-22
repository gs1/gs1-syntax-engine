/*
 * GS1 Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_winding.c
 *
 * @brief The `winding` linter ensures that the given data represents a
 * meaningful Winding Direction on a roll, either "0" (face out), "1" (face in)
 * or "9" (undefined).
 *
 * @remark The Winding Direction is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) section "Roll products -
 * width, length, core diameter, direction, splices: AI (8001)".
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component is the string "0", "1" or "9".
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_WINDING_DIRECTION if the data is not "0", "1" or "9".
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_winding(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	assert(data);

	/*
	 * The data must be either "0", "1" or "9".
	 *
	 */
	if (strcmp(data, "0") != 0 && strcmp(data, "1") != 0 && strcmp(data, "9") != 0)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INVALID_WINDING_DIRECTION,
			0,
			strlen(data)
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_winding(void)
{

	UNIT_TEST_PASS(gs1_lint_winding, "0");
	UNIT_TEST_PASS(gs1_lint_winding, "1");
	UNIT_TEST_FAIL(gs1_lint_winding, "2", GS1_LINTER_INVALID_WINDING_DIRECTION, "*2*");
	UNIT_TEST_FAIL(gs1_lint_winding, "3", GS1_LINTER_INVALID_WINDING_DIRECTION, "*3*");
	UNIT_TEST_FAIL(gs1_lint_winding, "4", GS1_LINTER_INVALID_WINDING_DIRECTION, "*4*");
	UNIT_TEST_FAIL(gs1_lint_winding, "5", GS1_LINTER_INVALID_WINDING_DIRECTION, "*5*");
	UNIT_TEST_FAIL(gs1_lint_winding, "6", GS1_LINTER_INVALID_WINDING_DIRECTION, "*6*");
	UNIT_TEST_FAIL(gs1_lint_winding, "7", GS1_LINTER_INVALID_WINDING_DIRECTION, "*7*");
	UNIT_TEST_FAIL(gs1_lint_winding, "8", GS1_LINTER_INVALID_WINDING_DIRECTION, "*8*");
	UNIT_TEST_PASS(gs1_lint_winding, "9");

	UNIT_TEST_FAIL(gs1_lint_winding, "", GS1_LINTER_INVALID_WINDING_DIRECTION, "**");
	UNIT_TEST_FAIL(gs1_lint_winding, "/", GS1_LINTER_INVALID_WINDING_DIRECTION, "*/*");
	UNIT_TEST_FAIL(gs1_lint_winding, ":", GS1_LINTER_INVALID_WINDING_DIRECTION, "*:*");
	UNIT_TEST_FAIL(gs1_lint_winding, "01", GS1_LINTER_INVALID_WINDING_DIRECTION, "*01*");

}

#endif  /* UNIT_TESTS */
