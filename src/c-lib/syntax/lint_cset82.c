/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2026 GS1 AISBL.
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
 * @file lint_cset82.c
 *
 * @brief The `cset82` linter ensures that the given data contains only
 * characters within the GS1 AI encodable character set 82 ("CSET 82").
 *
 * @remark CSET 82 is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) table "GS1 AI encodable
 * character set 82".
 *
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component conforms to `X..99` syntax.
 *
 * Note: The length of the component is validated by the framework that calls
 * this function.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_CSET82_CHARACTER if the data contains a
 *         non-CSET 82 character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset82(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	/*
	 * CSET 82 characters: !"%&'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
	 *
	 */
	static const uint64_t cset82_bitfield[] = {
		0x0000000067ffffff,		// "!", '"', "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", 0-9
		0x7fffffe17fffffe0,		// ":", ";", "<", "=", ">", "?", A-Z, "_", a-z
		0x0000000000000000,
		0x0000000000000000
	};

	size_t pos;

	assert(data);


	/*
	 * Validate character set using bitfield lookup
	 *
	 */
	for (pos = 0; pos < data_len; pos++) {
		int valid;
		GS1_LINTER_BITFIELD_LOOKUP((unsigned char)data[pos], cset82_bitfield, valid);
		if (GS1_LINTER_UNLIKELY(!valid))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_INVALID_CSET82_CHARACTER,
				pos,
				1
			);
	}

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_cset82(void)
{

	UNIT_TEST_PASS(gs1_lint_cset82, "");
	UNIT_TEST_PASS(gs1_lint_cset82, "!\"%&'()*+,-/012345");
	UNIT_TEST_PASS(gs1_lint_cset82, "6789:;<=>?ABCDEFGHIJ");
	UNIT_TEST_PASS(gs1_lint_cset82, "KLMNOPQRSTUVWXYZ_abc");
	UNIT_TEST_PASS(gs1_lint_cset82, "defghijklmnopqrstuvw");
	UNIT_TEST_PASS(gs1_lint_cset82, "xyz");

	UNIT_TEST_FAIL(gs1_lint_cset82, "ABC ", GS1_LINTER_INVALID_CSET82_CHARACTER, "ABC* *");
	UNIT_TEST_FAIL(gs1_lint_cset82, "AB C", GS1_LINTER_INVALID_CSET82_CHARACTER, "AB* *C");
	UNIT_TEST_FAIL(gs1_lint_cset82, " ABC", GS1_LINTER_INVALID_CSET82_CHARACTER, "* *ABC");

	UNIT_TEST_FAIL(gs1_lint_cset82, "\x01", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x01*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x02", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x02*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x03", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x03*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x04", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x04*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x05", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x05*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x06", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x06*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x07", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x07*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x08", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x08*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x09", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x09*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0A", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0A*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0B", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0B*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0C", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0C*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0D", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0D*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0E", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0E*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x0F", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x0F*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x10", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x10*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x11", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x11*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x12", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x12*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x13", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x13*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x14", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x14*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x15", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x15*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x16", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x16*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x17", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x17*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x18", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x18*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x19", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x19*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1A", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1A*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1B", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1B*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1C", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1C*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1D", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1D*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1E", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1E*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x1F", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x1F*");
	UNIT_TEST_FAIL(gs1_lint_cset82, " ",    GS1_LINTER_INVALID_CSET82_CHARACTER, "* *");
	UNIT_TEST_PASS(gs1_lint_cset82, "!");
	UNIT_TEST_PASS(gs1_lint_cset82, "\"");
	UNIT_TEST_FAIL(gs1_lint_cset82, "#",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*#*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "$",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*$*");
	UNIT_TEST_PASS(gs1_lint_cset82, "%");
	UNIT_TEST_PASS(gs1_lint_cset82, "&");
	UNIT_TEST_PASS(gs1_lint_cset82, "'");
	UNIT_TEST_PASS(gs1_lint_cset82, "(");
	UNIT_TEST_PASS(gs1_lint_cset82, ")");
	UNIT_TEST_PASS(gs1_lint_cset82, "*");
	UNIT_TEST_PASS(gs1_lint_cset82, "+");
	UNIT_TEST_PASS(gs1_lint_cset82, ",");
	UNIT_TEST_PASS(gs1_lint_cset82, "-");
	UNIT_TEST_PASS(gs1_lint_cset82, ".");
	UNIT_TEST_PASS(gs1_lint_cset82, "/");
	UNIT_TEST_PASS(gs1_lint_cset82, "0");
	UNIT_TEST_PASS(gs1_lint_cset82, "1");
	UNIT_TEST_PASS(gs1_lint_cset82, "2");
	UNIT_TEST_PASS(gs1_lint_cset82, "3");
	UNIT_TEST_PASS(gs1_lint_cset82, "4");
	UNIT_TEST_PASS(gs1_lint_cset82, "5");
	UNIT_TEST_PASS(gs1_lint_cset82, "6");
	UNIT_TEST_PASS(gs1_lint_cset82, "7");
	UNIT_TEST_PASS(gs1_lint_cset82, "8");
	UNIT_TEST_PASS(gs1_lint_cset82, "9");
	UNIT_TEST_PASS(gs1_lint_cset82, ":");
	UNIT_TEST_PASS(gs1_lint_cset82, ";");
	UNIT_TEST_PASS(gs1_lint_cset82, "<");
	UNIT_TEST_PASS(gs1_lint_cset82, "=");
	UNIT_TEST_PASS(gs1_lint_cset82, ">");
	UNIT_TEST_PASS(gs1_lint_cset82, "?");
	UNIT_TEST_FAIL(gs1_lint_cset82, "@",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*@*");
	UNIT_TEST_PASS(gs1_lint_cset82, "A");
	UNIT_TEST_PASS(gs1_lint_cset82, "B");
	UNIT_TEST_PASS(gs1_lint_cset82, "C");
	UNIT_TEST_PASS(gs1_lint_cset82, "D");
	UNIT_TEST_PASS(gs1_lint_cset82, "E");
	UNIT_TEST_PASS(gs1_lint_cset82, "F");
	UNIT_TEST_PASS(gs1_lint_cset82, "G");
	UNIT_TEST_PASS(gs1_lint_cset82, "H");
	UNIT_TEST_PASS(gs1_lint_cset82, "I");
	UNIT_TEST_PASS(gs1_lint_cset82, "J");
	UNIT_TEST_PASS(gs1_lint_cset82, "K");
	UNIT_TEST_PASS(gs1_lint_cset82, "L");
	UNIT_TEST_PASS(gs1_lint_cset82, "M");
	UNIT_TEST_PASS(gs1_lint_cset82, "N");
	UNIT_TEST_PASS(gs1_lint_cset82, "O");
	UNIT_TEST_PASS(gs1_lint_cset82, "P");
	UNIT_TEST_PASS(gs1_lint_cset82, "Q");
	UNIT_TEST_PASS(gs1_lint_cset82, "R");
	UNIT_TEST_PASS(gs1_lint_cset82, "S");
	UNIT_TEST_PASS(gs1_lint_cset82, "T");
	UNIT_TEST_PASS(gs1_lint_cset82, "U");
	UNIT_TEST_PASS(gs1_lint_cset82, "V");
	UNIT_TEST_PASS(gs1_lint_cset82, "W");
	UNIT_TEST_PASS(gs1_lint_cset82, "X");
	UNIT_TEST_PASS(gs1_lint_cset82, "Y");
	UNIT_TEST_PASS(gs1_lint_cset82, "Z");
	UNIT_TEST_FAIL(gs1_lint_cset82, "[",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*[*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\\",   GS1_LINTER_INVALID_CSET82_CHARACTER, "*\\*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "]",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*]*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "^",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*^*");
	UNIT_TEST_PASS(gs1_lint_cset82, "_");
	UNIT_TEST_FAIL(gs1_lint_cset82, "`",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*`*");
	UNIT_TEST_PASS(gs1_lint_cset82, "a");
	UNIT_TEST_PASS(gs1_lint_cset82, "b");
	UNIT_TEST_PASS(gs1_lint_cset82, "c");
	UNIT_TEST_PASS(gs1_lint_cset82, "d");
	UNIT_TEST_PASS(gs1_lint_cset82, "e");
	UNIT_TEST_PASS(gs1_lint_cset82, "f");
	UNIT_TEST_PASS(gs1_lint_cset82, "g");
	UNIT_TEST_PASS(gs1_lint_cset82, "h");
	UNIT_TEST_PASS(gs1_lint_cset82, "i");
	UNIT_TEST_PASS(gs1_lint_cset82, "j");
	UNIT_TEST_PASS(gs1_lint_cset82, "k");
	UNIT_TEST_PASS(gs1_lint_cset82, "l");
	UNIT_TEST_PASS(gs1_lint_cset82, "m");
	UNIT_TEST_PASS(gs1_lint_cset82, "n");
	UNIT_TEST_PASS(gs1_lint_cset82, "o");
	UNIT_TEST_PASS(gs1_lint_cset82, "p");
	UNIT_TEST_PASS(gs1_lint_cset82, "q");
	UNIT_TEST_PASS(gs1_lint_cset82, "r");
	UNIT_TEST_PASS(gs1_lint_cset82, "s");
	UNIT_TEST_PASS(gs1_lint_cset82, "t");
	UNIT_TEST_PASS(gs1_lint_cset82, "u");
	UNIT_TEST_PASS(gs1_lint_cset82, "v");
	UNIT_TEST_PASS(gs1_lint_cset82, "w");
	UNIT_TEST_PASS(gs1_lint_cset82, "x");
	UNIT_TEST_PASS(gs1_lint_cset82, "y");
	UNIT_TEST_PASS(gs1_lint_cset82, "z");
	UNIT_TEST_FAIL(gs1_lint_cset82, "{",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*{*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "|",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*|*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "}",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*}*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "~",    GS1_LINTER_INVALID_CSET82_CHARACTER, "*~*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x7F", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x7F*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x80", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x80*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x81", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x81*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x82", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x82*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x83", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x83*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x84", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x84*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x85", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x85*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x86", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x86*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x87", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x87*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x88", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x88*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x89", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x89*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8A", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8A*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8B", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8B*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8C", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8C*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8D", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8D*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8E", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8E*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x8F", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x8F*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x90", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x90*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x91", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x91*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x92", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x92*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x93", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x93*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x94", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x94*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x95", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x95*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x96", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x96*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x97", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x97*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x98", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x98*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x99", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x99*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9A", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9A*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9B", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9B*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9C", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9C*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9D", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9D*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9E", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9E*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\x9F", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\x9F*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xA9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xA9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAD", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAD*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xAF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xAF*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xB9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xB9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBD", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBD*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xBF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xBF*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xC9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xC9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCD", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCD*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xCF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xCF*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xD9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xD9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDD", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDD*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xDF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xDF*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xE9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xE9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xEA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xEA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xEB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xEB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xEC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xEC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xED", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xED*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xEE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xEE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xEF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xEF*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF0", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF0*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF1", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF1*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF2", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF2*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF3", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF3*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF4", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF4*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF5", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF5*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF6", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF6*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF7", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF7*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF8", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF8*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xF9", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xF9*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFA", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFA*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFB", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFB*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFC", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFC*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFD", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFD*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFE", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFE*");
	UNIT_TEST_FAIL(gs1_lint_cset82, "\xFF", GS1_LINTER_INVALID_CSET82_CHARACTER, "*\xFF*");

}

#endif  /* UNIT_TESTS */
