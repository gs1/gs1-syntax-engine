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
#include <stddef.h>
#include <stdint.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that the AI component has a valid alphanumeric check
 * character pair.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
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
 *
 * @note The return value `GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION`
 * represents exceeding an implementation-specific limit. Some implementations
 * may choose to support longer data inputs.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csumalpha(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
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
	 * Sequence of all characters in CSET 32, ordered by weight.
	 */
	static const char* const cset32 = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";

	/*
	 * CSET82 character weights lookup table (0 = invalid; weight off by 1)
	 */
	static const unsigned char cset82_weights[256] = {
		['!'] = 1, ['"'] = 2, ['%'] = 3, ['&'] = 4, ['\''] = 5, ['('] = 6,
		[')'] = 7, ['*'] = 8, ['+'] = 9, [','] = 10, ['-'] = 11, ['.'] = 12,
		['/'] = 13, ['0'] = 14, ['1'] = 15, ['2'] = 16, ['3'] = 17, ['4'] = 18,
		['5'] = 19, ['6'] = 20, ['7'] = 21, ['8'] = 22, ['9'] = 23, [':'] = 24,
		[';'] = 25, ['<'] = 26, ['='] = 27, ['>'] = 28, ['?'] = 29, ['A'] = 30,
		['B'] = 31, ['C'] = 32, ['D'] = 33, ['E'] = 34, ['F'] = 35, ['G'] = 36,
		['H'] = 37, ['I'] = 38, ['J'] = 39, ['K'] = 40, ['L'] = 41, ['M'] = 42,
		['N'] = 43, ['O'] = 44, ['P'] = 45, ['Q'] = 46, ['R'] = 47, ['S'] = 48,
		['T'] = 49, ['U'] = 50, ['V'] = 51, ['W'] = 52, ['X'] = 53, ['Y'] = 54,
		['Z'] = 55, ['_'] = 56, ['a'] = 57, ['b'] = 58, ['c'] = 59, ['d'] = 60,
		['e'] = 61, ['f'] = 62, ['g'] = 63, ['h'] = 64, ['i'] = 65, ['j'] = 66,
		['k'] = 67, ['l'] = 68, ['m'] = 69, ['n'] = 70, ['o'] = 71, ['p'] = 72,
		['q'] = 73, ['r'] = 74, ['s'] = 75, ['t'] = 76, ['u'] = 77, ['v'] = 78,
		['w'] = 79, ['x'] = 80, ['y'] = 81, ['z'] = 82
	};

	size_t pos;
	uint32_t sum = 0;	/* Sufficient for 97-prime implementation */

	assert(data);


	/*
	 * Constrain data to the number of primes that we have.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len > sizeof(primes) / sizeof(primes[0]) + 2))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION,
			0,
			data_len
		);

	/*
	 * Data must include at least the check character pair.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len < 2))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR,
			0,
			data_len
		);

	/*
	 * Handling the two-character case now avoids conditionals later.
	 *
	 */
	if (data_len == 2) {
		if (GS1_LINTER_UNLIKELY(data[0] != '2' || data[1] != '2'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_INCORRECT_CHECK_PAIR,
				data_len - 2,
				2
			);
		GS1_LINTER_RETURN_OK;
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
	for (pos = 0; pos < data_len - 2; pos++) {
		if (GS1_LINTER_UNLIKELY(cset82_weights[(unsigned char)data[pos]] == 0))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_INVALID_CSET82_CHARACTER,
				pos,
				1
			);
		sum += (unsigned int)(cset82_weights[(unsigned char)data[pos]] - 1) * primes[data_len - 3 - pos];
	}
	sum %= 1021;	/* Overflow not possible with uint32_t */

	if (GS1_LINTER_UNLIKELY(data[data_len-2] != cset32[sum >> 5] || data[data_len-1] != cset32[sum & 31]))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INCORRECT_CHECK_PAIR,
			data_len - 2,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_csumalpha(void)
{

	UNIT_TEST_FAIL(gs1_lint_csumalpha, "", GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR, "**");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "2", GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR, "*2*");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "22");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "33", GS1_LINTER_INCORRECT_CHECK_PAIR, "*33*");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "!22");
	UNIT_TEST_PASS(gs1_lint_csumalpha, "!!22");

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

	UNIT_TEST_PASS(gs1_lint_csumalpha, "12345678901234567890123456789012345678901234567890"
					   "12345678901234567890123456789012345678901234567HA");  // len = 99

	UNIT_TEST_PASS(gs1_lint_csumalpha, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
					   "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzUA");  // Maximum implementation sum

	UNIT_TEST_FAIL(gs1_lint_csumalpha, "12345678901234567890123456789012345678901234567890"
					   "123456789012345678901234567890123456789012345678ZZ",
					   GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION,
					   "*12345678901234567890123456789012345678901234567890"
					   "123456789012345678901234567890123456789012345678ZZ*");  // len = 100, exceeds implementation

	UNIT_TEST_FAIL(gs1_lint_csumalpha, " 2345678901234567890123NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "* *2345678901234567890123NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "123456789 1234567890123NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "123456789* *1234567890123NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "1234567890123456789012 NT", GS1_LINTER_INVALID_CSET82_CHARACTER, "1234567890123456789012* *NT");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "12345678901234567890123 T", GS1_LINTER_INCORRECT_CHECK_PAIR, "12345678901234567890123* T*");
	UNIT_TEST_FAIL(gs1_lint_csumalpha, "12345678901234567890123N ", GS1_LINTER_INCORRECT_CHECK_PAIR, "12345678901234567890123*N *");

}

#endif  /* UNIT_TESTS */
