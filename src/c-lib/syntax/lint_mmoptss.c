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
 * @file lint_mmoptss.c
 *
 * @brief The `mmoptss` linter ensures that the data represents a meaningful
 * time fragment as minutes and optional seconds, in MMSS or MM format.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to one of the MMSS or MM
 * formats.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_MMSS_INVALID_LENGTH if the data neither two not two characters.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MINUTE if the data contains an invalid minute.
 * @return #GS1_LINTER_ILLEGAL_SECOND if the data contains an invalid second.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mmoptss(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t len, pos;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be two or four characters.
	 *
	 */
	if (len != 2 && len != 4) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_MMSS_INVALID_LENGTH;
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
	 * Validate the minute.
	 *
	 */
	if ((data[0] - '0') * 10 + (data[1] - '0') > 59) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_MINUTE;
	}

	/*
	 * Validate the second, if it is provided.
	 *
	 */
	if (len == 4 && (data[2] - '0') * 10 + (data[3] - '0') > 59) {
		if (err_pos) *err_pos = 2;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_SECOND;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_mmoptss(void)
{

	UNIT_TEST_PASS(gs1_lint_mmoptss, "00");
	UNIT_TEST_PASS(gs1_lint_mmoptss, "59");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "60", GS1_LINTER_ILLEGAL_MINUTE, "*60*");

	UNIT_TEST_PASS(gs1_lint_mmoptss, "0000");
	UNIT_TEST_PASS(gs1_lint_mmoptss, "5900");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "6000", GS1_LINTER_ILLEGAL_MINUTE, "*60*00");
	UNIT_TEST_PASS(gs1_lint_mmoptss, "0059");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "0060", GS1_LINTER_ILLEGAL_SECOND, "00*60*");

	UNIT_TEST_FAIL(gs1_lint_mmoptss, "x0", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "0x", GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_mmoptss, "x000", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*000");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "0x00", GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*00");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "00x0", GS1_LINTER_NON_DIGIT_CHARACTER, "00*x*0");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "000x", GS1_LINTER_NON_DIGIT_CHARACTER, "000*x*");

	UNIT_TEST_FAIL(gs1_lint_mmoptss, "",      GS1_LINTER_MMSS_INVALID_LENGTH, "**");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "1",     GS1_LINTER_MMSS_INVALID_LENGTH, "*1*");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "111",   GS1_LINTER_MMSS_INVALID_LENGTH, "*111*");
	UNIT_TEST_FAIL(gs1_lint_mmoptss, "11111", GS1_LINTER_MMSS_INVALID_LENGTH, "*11111*");

}

#endif  /* UNIT_TESTS */
