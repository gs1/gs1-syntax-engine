/*
 * GS1 Syntax Dictionary. Copyright (c) 2024 GS1 AISBL.
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
 * @file lint_iso5218.c
 *
 * @brief The `iso5218` linter ensures that the given data represents a ISO/IEC
 * 5218 biological sex code, either "0" (not known), "1" (male), "2" (female)
 * or "9" (not applicable).
 *
 * @remark The biological sex codes are defined by [ISO/IEC 5218: Information
 * technology - Codes for the representation of human sexes](https://www.iso.org/standard/81682.html).
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to validate that an AI component is the string "0", "1", "2" or "9".
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE if the data is not "0", "1", "2" or "9".
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso5218(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	assert(data);

	/*
	 * The data must be either "0", "1", "2" or "9".
	 *
	 */
	if (strcmp(data, "0") != 0 && strcmp(data, "1") != 0 &&
	    strcmp(data, "2") != 0 && strcmp(data, "9") != 0) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = strlen(data);
		return GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_iso5218(void)
{

	UNIT_TEST_PASS(gs1_lint_iso5218, "0");
	UNIT_TEST_PASS(gs1_lint_iso5218, "1");
	UNIT_TEST_PASS(gs1_lint_iso5218, "2");
	UNIT_TEST_PASS(gs1_lint_iso5218, "9");

	UNIT_TEST_FAIL(gs1_lint_iso5218, "", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "**");
	UNIT_TEST_FAIL(gs1_lint_iso5218, "/", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "*/*");
	UNIT_TEST_FAIL(gs1_lint_iso5218, "3", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "*3*");
	UNIT_TEST_FAIL(gs1_lint_iso5218, "8", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "*8*");
	UNIT_TEST_FAIL(gs1_lint_iso5218, ":", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "*:*");
	UNIT_TEST_FAIL(gs1_lint_iso5218, "01", GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE, "*01*");

}

#endif  /* UNIT_TESTS */
