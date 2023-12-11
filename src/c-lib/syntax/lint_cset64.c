/*
 * GS1 Syntax Dictionary. Copyright (c) 2023 GS1 AISBL.
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
 * @file lint_cset64.c
 *
 * @brief The `cset64` linter ensures that the given data contains only
 * characters within the GS1 AI encodable character set 64 ("CSET 64").
 *
 * @remark CSET 64 is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) table "GS1 AI encodable
 * character set 64 (file-safe / URI-safe base64)".
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to validate that an AI component conforms to `Z..99` syntax, and has
 * valid padding if provided.
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
 * @return #GS1_LINTER_INVALID_CSET64_CHARACTER if the data contains a
 *         non-CSET 64 character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset64(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	/*
	 * All characters in "CSET 64", the 64 character alphabet.
	 *
	 */
	static const char* const cset64 =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789-_";

	size_t pads, len, pos;

	assert(data);

	/*
	 * Count the number of padding characters, which are optional.
	 *
	 */
	for (pads = 0, len = strlen(data);
	     len > 0 && data[len-1] == '=';
	     pads++, len--);

	if (pads > 2 || (pads > 0 && (len + pads) % 3 != 0)) {
		if (err_pos) *err_pos = len;
		if (err_len) *err_len = pads;
		return GS1_LINTER_INVALID_CSET64_PADDING;
	}

	/*
	 * In what remains, any character outside of CSET 64 is illegal.
	 *
	 */
	if ((pos = strspn(data, cset64)) < len) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_INVALID_CSET64_CHARACTER;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_cset64(void)
{

	UNIT_TEST_PASS(gs1_lint_cset64, "");
	UNIT_TEST_PASS(gs1_lint_cset64, "123");
	UNIT_TEST_PASS(gs1_lint_cset64, "12");	// Padding is optional
	UNIT_TEST_PASS(gs1_lint_cset64, "12=");
	UNIT_TEST_PASS(gs1_lint_cset64, "1==");

	UNIT_TEST_FAIL(gs1_lint_cset64, "===", GS1_LINTER_INVALID_CSET64_PADDING, "*===*");

	UNIT_TEST_FAIL(gs1_lint_cset64, "123=", GS1_LINTER_INVALID_CSET64_PADDING, "123*=*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12==", GS1_LINTER_INVALID_CSET64_PADDING, "12*==*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "1===", GS1_LINTER_INVALID_CSET64_PADDING, "1*===*");

	UNIT_TEST_FAIL(gs1_lint_cset64, "1234=", GS1_LINTER_INVALID_CSET64_PADDING, "1234*=*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "123==", GS1_LINTER_INVALID_CSET64_PADDING, "123*==*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12===", GS1_LINTER_INVALID_CSET64_PADDING, "12*===*");

	UNIT_TEST_PASS(gs1_lint_cset64, "");
	UNIT_TEST_PASS(gs1_lint_cset64, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	UNIT_TEST_PASS(gs1_lint_cset64, "abcdefghijklmnopqrstuvwxyz");
	UNIT_TEST_PASS(gs1_lint_cset64, "0123456789-_");

	UNIT_TEST_FAIL(gs1_lint_cset64, " ",    GS1_LINTER_INVALID_CSET64_CHARACTER, "* *");
	UNIT_TEST_FAIL(gs1_lint_cset64, "ABC ", GS1_LINTER_INVALID_CSET64_CHARACTER, "ABC* *");
	UNIT_TEST_FAIL(gs1_lint_cset64, "AB C", GS1_LINTER_INVALID_CSET64_CHARACTER, "AB* *C");
	UNIT_TEST_FAIL(gs1_lint_cset64, " ABC", GS1_LINTER_INVALID_CSET64_CHARACTER, "* *ABC");

}

#endif  /* UNIT_TESTS */
