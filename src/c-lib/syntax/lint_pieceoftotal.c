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
 * @file lint_pieceoftotal.c
 *
 * @brief The `pieceoftotal` linter ensures that the data represents a
 * meaningful piece of total, i.e. a concatenation of non-zero piece number
 * follow by an equal-width, non-zero total number of pieces.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to a PPTT format, where PP and
 * TT have equal width.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL if the data is not an even length.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ZERO_PIECE_NUMBER if the data contains a piece number with a zero value.
 * @return #GS1_LINTER_ZERO_TOTAL_PIECES if the data contains a total piece count with a zero value.
 * @return #GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL if the data contains a piece number that is larger than the total piece count.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_pieceoftotal(const char* const data, size_t* const err_pos, size_t* const err_len)
{

/// \cond
#define P(i)	data[i]
#define T(i)	data[len / 2 + i]
/// \endcond

	size_t pos, len, i;
	int pieceiszero, totaliszero, compare;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be a non-zero, even number of characters.
	 *
	 */
	if (len == 0 || len % 2 != 0) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL;
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
	 * Determine whether either the piece number or total piece count is
	 * zero and whether the piece number exceeds the total piece count.
	 *
	 */
	pieceiszero = totaliszero = 1;
	compare = 0;				/* -1:P<T ; 0:P==T ; 1:P>T */
	for (i = 0; i < len / 2; i++) {
		if (pieceiszero && P(i) != '0') pieceiszero = 0;
		if (totaliszero && T(i) != '0') totaliszero = 0;
		if (!compare && P(i) != T(i)) compare = P(i) < T(i) ? -1 : 1;
	}

	/*
	 * Neither piece nor total may be zero.
	 *
	 */
	if (pieceiszero || totaliszero) {
		if (err_pos) *err_pos = pieceiszero ? 0 : len / 2;
		if (err_len) *err_len = len / 2;
		return pieceiszero ? GS1_LINTER_ZERO_PIECE_NUMBER : GS1_LINTER_ZERO_TOTAL_PIECES;
	}

	/*
	 * The piece number must not exceed the total piece count.
	 *
	 */
	if (compare == 1) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_pieceoftotal(void)
{

	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "11");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "13");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "23");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "33");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "33");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "89");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "99");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "0101");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "0103");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "0203");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "0303");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "9899");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "9999");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "001001");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "001003");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "002003");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "003003");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "998999");
	UNIT_TEST_PASS(gs1_lint_pieceoftotal, "999999");

	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "",          GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "**");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "1",         GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "*1*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "111",       GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "*111*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "11111",     GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "1111111",   GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "*1111111*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "111111111", GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL, "*111111111*");

	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "x5555555", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*5555555");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "5x555555", GS1_LINTER_NON_DIGIT_CHARACTER, "5*x*555555");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "55x55555", GS1_LINTER_NON_DIGIT_CHARACTER, "55*x*55555");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "555x5555", GS1_LINTER_NON_DIGIT_CHARACTER, "555*x*5555");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "5555x555", GS1_LINTER_NON_DIGIT_CHARACTER, "5555*x*555");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "55555x55", GS1_LINTER_NON_DIGIT_CHARACTER, "55555*x*55");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "555555x5", GS1_LINTER_NON_DIGIT_CHARACTER, "555555*x*5");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "5555555x", GS1_LINTER_NON_DIGIT_CHARACTER, "5555555*x*");

	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "01",     GS1_LINTER_ZERO_PIECE_NUMBER, "*0*1");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "00",     GS1_LINTER_ZERO_PIECE_NUMBER, "*0*0");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "10",     GS1_LINTER_ZERO_TOTAL_PIECES, "1*0*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "0001",   GS1_LINTER_ZERO_PIECE_NUMBER, "*00*01");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "0000",   GS1_LINTER_ZERO_PIECE_NUMBER, "*00*00");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "0100",   GS1_LINTER_ZERO_TOTAL_PIECES, "01*00*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "000001", GS1_LINTER_ZERO_PIECE_NUMBER, "*000*001");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "000000", GS1_LINTER_ZERO_PIECE_NUMBER, "*000*000");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "001000", GS1_LINTER_ZERO_TOTAL_PIECES, "001*000*");

	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "21",     GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*21*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "98",     GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*98*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "0908",   GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*0908*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "1009",   GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*1009*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "1110",   GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*1110*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "9998",   GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*9998*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "002001", GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*002001*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "099098", GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*099098*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "100099", GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*100099*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "101100", GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*101100*");
	UNIT_TEST_FAIL(gs1_lint_pieceoftotal, "999998", GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL, "*999998*");

}

#endif  /* UNIT_TESTS */
