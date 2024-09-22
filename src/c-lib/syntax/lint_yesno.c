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
 * @file lint_yesno.c
 *
 * @brief The `yesno` linter ensures that the given data represents a
 * meaningful boolean state, either "0" (no) or "1" (yes).
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component is either the string "0" or "1".
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NOT_ZERO_OR_ONE if the data is not "0" or "1".
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yesno(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	assert(data);

	/*
	 * The data must be either "0" or "1".
	 *
	 */
	if (strcmp(data, "0") != 0 && strcmp(data, "1") != 0)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NOT_ZERO_OR_ONE,
			0,
			strlen(data)
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yesno(void)
{

	UNIT_TEST_PASS(gs1_lint_yesno, "0");
	UNIT_TEST_PASS(gs1_lint_yesno, "1");

	UNIT_TEST_FAIL(gs1_lint_yesno, "", GS1_LINTER_NOT_ZERO_OR_ONE, "**");
	UNIT_TEST_FAIL(gs1_lint_yesno, "/", GS1_LINTER_NOT_ZERO_OR_ONE, "*/*");
	UNIT_TEST_FAIL(gs1_lint_yesno, "2", GS1_LINTER_NOT_ZERO_OR_ONE, "*2*");
	UNIT_TEST_FAIL(gs1_lint_yesno, "01", GS1_LINTER_NOT_ZERO_OR_ONE, "*01*");

}

#endif  /* UNIT_TESTS */
