/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2024-2024 GS1 AISBL.
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
 * @file lint_posinseqslash.c
 *
 * @brief The `posinseqslash` linter ensures that the data represents a
 * meaningful position in a sequence in the format "`<pos>/<end>`".
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to a "`<pos>/<end>`" format for
 * variable width `<pos>` and `<end>`.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED if the data does not conform to "`<pos>/<end>`" format.
 * @return #GS1_LINTER_ILLEGAL_ZERO_PREFIX if either the position number or the end number are zero or contain a zero prefix.
 * @return #GS1_LINTER_POSITION_EXCEEDS_END if the data contains a position number that is larger than the end position.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_posinseqslash(const char* const data, size_t* const err_pos, size_t* const err_len)
{

/// \cond
#define P(i)    data[i]
#define E(i)    data[i + pos + 1]
/// \endcond

	size_t pos, len;

	assert(data);

	len = strlen(data);

	/*
	 * Determine that the format is "<pos>/<end>".
	 *
	 */
	pos = strspn(data, "0123456789");
	if (pos == 0 || pos >= len - 1 || data[pos] != '/' || strspn(data + pos + 1, "0123456789") != len - pos - 1)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED,
			0,
			len
		);

	/*
	 * Ensure position number is non-zero and does not have zero prefix.
	 *
	 */
	if (P(0) == '0')
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_ZERO_PREFIX,
			0,
			pos
		);

	/*
	 * Ensure end number is non-zero and does not have zero prefix.
	 *
	 */
	if (E(0) == '0')
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_ZERO_PREFIX,
			pos + 1,
			len - pos - 1
		);

	/*
	 * Determine whether the position exceeds the end.
	 *
	 */
	if (pos == len - pos - 1) {
		size_t i;
		int compare = 0;		/* -1:P<E ; 0:P==E ; 1:P>E */
		for (i = 0; i < pos && !compare; i++)
			if (P(i) != E(i))
				compare = P(i) < E(i) ? -1 : 1;
		if (compare == 1)
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_POSITION_EXCEEDS_END,
				0,
				len
			);
	} else if (pos > len - pos - 1)
		/* Non-zero prefix, so a length check is sufficient. */
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_POSITION_EXCEEDS_END,
			0,
			len
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_posinseqslash(void)
{

	UNIT_TEST_PASS(gs1_lint_posinseqslash, "1/1");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "1/3");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "2/3");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "3/3");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "8/9");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "9/9");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "9/10");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "10/10");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "1/100");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "100/100");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "98/99");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "99/99");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "998/999");
	UNIT_TEST_PASS(gs1_lint_posinseqslash, "999/999");

	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "",        GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "**");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "/",       GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*/*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1",       GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*1*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "x",       GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*x*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1/",      GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*1/*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "x/",      GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*x/*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1//",     GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*1//*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "/1",      GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*/1*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "/x",      GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*/x*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "//1",     GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*//1*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "//",      GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*//*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1//2",    GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*1//2*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "x/y",     GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*x/y*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "x//y",    GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED, "*x//y*");

	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "0/1",     GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*/1");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1/0",     GS1_LINTER_ILLEGAL_ZERO_PREFIX, "1/*0*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "0/0",     GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*/0");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "1/02",    GS1_LINTER_ILLEGAL_ZERO_PREFIX, "1/*02*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "01/2",    GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*01*/2");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "01/02",   GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*01*/02");

	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "2/1",     GS1_LINTER_POSITION_EXCEEDS_END, "*2/1*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "9/8",     GS1_LINTER_POSITION_EXCEEDS_END, "*9/8*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "10/9",    GS1_LINTER_POSITION_EXCEEDS_END, "*10/9*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "11/10",   GS1_LINTER_POSITION_EXCEEDS_END, "*11/10*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "99/98",   GS1_LINTER_POSITION_EXCEEDS_END, "*99/98*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "100/99",  GS1_LINTER_POSITION_EXCEEDS_END, "*100/99*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "101/100", GS1_LINTER_POSITION_EXCEEDS_END, "*101/100*");
	UNIT_TEST_FAIL(gs1_lint_posinseqslash, "999/998", GS1_LINTER_POSITION_EXCEEDS_END, "*999/998*");

}

#endif  /* UNIT_TESTS */
