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
 * @file lint_csumalpha.c
 *
 * @brief The `csumalpha` linter ensures that the data has a valid alphanumeric
 * check character pair.
 *
 * @remark The process for validating the GS1 alphanumeric check character pair
 * is described in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) section "Check character
 * calculation (for alphanumeric keys)".
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that the AI component has a valid alphanumeric check
 * character pair.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INCORRECT_CHECK_PAIR if the data contains an incorrect
 *         check character pair.
 * @return #GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR if the data is too short.
 * @return #GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION if the data is
 *         too long for this implementation of the check character pair
 *         algorithm.
 *         `GS1_LINTER_INVALID_CSET82_CHARACTER` if any of the data characters
 *         are not in CSET 82.
 *         `GS1_LINTER_INVALID_CSET32_CHARACTER` if any of the data characters
 *         are not in CSET 32.
 *
 * @note The return value `GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION`
 * represents exceeding an implementation-specific limit. Some implmenetations
 * may choose to support longer data inputs.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csumalpha(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	/*
	 *  Set of prime weights used to derive alpha check characters.
	 *
	 *  Here we have the first 97 since that is the maximum length of any
	 *  AI is currently 99. Implementations may choose to support longer
	 *  data inputs.
	 *
	 */
	static const unsigned int primes[] = {
		  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,
		 41,  43,  47,  53,  59,  61,  67,  71,  73,  79,  83,  89,
		 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
		157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223,
		227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
		283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359,
		367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
		439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503,
		509
	};

	/*
	 * Sequence of all characters in CSET 82, ordered by weight.
	 *
	 */
	static const char* const cset82 =
		"!\"%&'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
		"abcdefghijklmnopqrstuvwxyz";

	/*
	 * Sequence of all characters in CSET 32, ordered by weight.
	 *
	 */
	static const char* const cset32 = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";

	size_t i, pos, len;
	unsigned int sum = 0;
	const unsigned int *p;

	assert(data);

	len = strlen(data);

	/*
	 * Data must include at least the check character pair.
	 *
	 */
	if (len < 2) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR;
	}

	/*
	 * Constrain the length of the data to the number of primes that we
	 * have.
	 *
	 */
	if (len > sizeof(primes) / sizeof(primes[0])) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION;
	}

	/*
	 * Ensure that the data characters are in CSET 82
	 *
	 */
	if ((pos = strspn(data, cset82)) < len - 2) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_INVALID_CSET82_CHARACTER;
	}

	/*
	 * Ensure that the check characters are in CSET 32
	 *
	 */
	if ((pos = strspn(&data[len - 2], cset32)) != 2) {
		if (err_pos) *err_pos = len - 2 + pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_INVALID_CSET32_CHARACTER;
	}

	/*
	 * Sum of data-character values weighted by increasing prime values,
	 * from right to left, modulo 1021.
	 *
	 * The check characters are valid when their values match the above
	 * data checksum with its 10-bit value split evenly into two 5-bit
	 * values.
	 *
	 * The data character values are defined by their respective positions
	 * in CSET 82.
	 *
	 * The check character pair values are defined by their respective
	 * positions in CSET 32.
	 *
	 */
	p = primes + len - 3;
	for (i = 0; i < len - 2; i++)
		sum += (unsigned int)(strchr(cset82, data[i]) - cset82) * *p--;
	sum %= 1021;

	if (data[i] != cset32[sum >> 5] || data[i+1] != cset32[sum & 31]) {
		if (err_pos) *err_pos = len - 2;
		if (err_len) *err_len = 2;
		return GS1_LINTER_INCORRECT_CHECK_PAIR;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_csumalpha(void)
{

	UNIT_TEST_FAIL(gs1_lint_csumalpha, "", GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR, "**");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "0", GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR, "*0*");

	UNIT_TEST_PASS(gs1_lint_csumalpha, "1987654Ad4X4bL5ttr2310c2K");

	UNIT_TEST_FAIL(gs1_lint_csumalpha, "1987654Ad4X4bL5ttr2310cXK", GS1_LINTER_INCORRECT_CHECK_PAIR, "1987654Ad4X4bL5ttr2310c*XK*");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "1987654Ad4X4bL5ttr2310c2X", GS1_LINTER_INCORRECT_CHECK_PAIR, "1987654Ad4X4bL5ttr2310c*2X*");

	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345678901234567890123NT");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_ABCDEFGHIJKLMCP");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_NOPQRSTUVWXYZDN");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_abcdefghijklmN3");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_nopqrstuvwxyzP2");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_!\"%&'()*+,-./LC");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345_0123456789:;<=>?62");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "7907665Bm8v2AB");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "97850l6KZm0yCD");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "225803106GSpEF");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "149512464PM+GH");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "62577B8fRG7HJK");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "515942070CYxLM");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "390800494sP6NP");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "386830132uO+QR");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "53395376X1:nST");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "957813138Sb6UV");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "530790no0qOgWX");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "62185314IvwmYZ");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "23956qk1&dB!23");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "794394895ic045");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "57453Uq3qA<H67");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "62185314IvwmYZ");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "0881063PhHvY89");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "00000!HV");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "99999zzzzzzzzzzzzzzzzzzT2");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "99999zzzzzzzzzzzzzzzzzzT2");

	UNIT_TEST_FAIL(gs1_lint_csumalpha, " 2345678901234567890123NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "* *2345678901234567890123NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "123456789 1234567890123NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "123456789* *1234567890123NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "1234567890123456789012 NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "1234567890123456789012* *NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "12345678901234567890123 T", GS1_LINTER_INVALID_CSET32_CHARACTER, "12345678901234567890123* *T");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "12345678901234567890123N ", GS1_LINTER_INVALID_CSET32_CHARACTER, "12345678901234567890123N* *");

}

#endif  /* UNIT_TESTS */
