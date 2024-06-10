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
 * @file lint_cset39.c
 *
 * @brief The `cset39` linter ensures that the given data contains only
 * characters within GS1 AI encodable character set 39 ("CSET 39").
 *
 * @remark CSET 39 is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) table "GS1 AI encodable
 * character set 39".
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to validate that an AI component conforms to `C..99` syntax.
 *
 * Note: The length of the component is validated by the framework that calls
 * this function.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_CSET39_CHARACTER if the data contains a
 *         non-CSET 39 character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset39(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	/*
	 * All characters in "CSET 39", the 39 character alphabet.
	 *
	 */
	static const char* const cset39 =
		"#-/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	size_t pos;

	assert(data);

	/*
	 * Any character outside of CSET 39 is illegal.
	 *
	 */
	if ((pos = strspn(data, cset39)) != strlen(data)) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_INVALID_CSET39_CHARACTER;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_cset39(void)
{

	UNIT_TEST_PASS(gs1_lint_cset39, "");
	UNIT_TEST_PASS(gs1_lint_cset39, "#-/0123456789ABCDEFG");
	UNIT_TEST_PASS(gs1_lint_cset39, "HIJKLMNOPQRSTUVWXYZ");

	UNIT_TEST_FAIL(gs1_lint_cset39, "_",    GS1_LINTER_INVALID_CSET39_CHARACTER, "*_*");
	UNIT_TEST_FAIL(gs1_lint_cset39, "ABC_", GS1_LINTER_INVALID_CSET39_CHARACTER, "ABC*_*");
	UNIT_TEST_FAIL(gs1_lint_cset39, "AB_C", GS1_LINTER_INVALID_CSET39_CHARACTER, "AB*_*C");
	UNIT_TEST_FAIL(gs1_lint_cset39, "_ABC", GS1_LINTER_INVALID_CSET39_CHARACTER, "*_*ABC");

}

#endif  /* UNIT_TESTS */
