/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2023-2024 GS1 AISBL.
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
#include "gs1syntaxdictionary-utils.h"


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
	 * CSET 64 characters: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_
	 *
	 */
	static const uint64_t cset64_bitfield[] = {
		0x000000000004ffc0,		// 0-9, "-"
		0x7fffffe17fffffe0,		// A-Z, "_", a-z
		0x0000000000000000,
		0x0000000000000000
	};

	size_t pads, len, pos;

	assert(data);

	/*
	 * Count padding characters from the end
	 *
	 */
	len = strlen(data);
	for (pads = 0; pads < len && data[len - pads - 1] == '='; pads++);
	len -= pads;

	if (GS1_LINTER_UNLIKELY(pads > 2 || (pads > 0 && (len + pads) % 3 != 0)))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INVALID_CSET64_PADDING,
			len,
			pads
		);

	/*
	 * Validate character set using bitfield lookup
	 *
	 */
	for (pos = 0; pos < len; pos++) {
		int valid = 0;
		GS1_LINTER_BITFIELD_LOOKUP((unsigned char)data[pos], cset64_bitfield);
		if (GS1_LINTER_UNLIKELY(!valid))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_INVALID_CSET64_CHARACTER,
				pos,
				1
			);
	}

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_cset64(void)
{

	UNIT_TEST_PASS(gs1_lint_cset64, "");

	// Padding is optional
	UNIT_TEST_PASS(gs1_lint_cset64, "1");
	UNIT_TEST_PASS(gs1_lint_cset64, "12");

	UNIT_TEST_PASS(gs1_lint_cset64, "123");
	UNIT_TEST_PASS(gs1_lint_cset64, "12=");
	UNIT_TEST_PASS(gs1_lint_cset64, "1==");
	UNIT_TEST_FAIL(gs1_lint_cset64, "===", GS1_LINTER_INVALID_CSET64_PADDING, "*===*");

	UNIT_TEST_FAIL(gs1_lint_cset64, "123=", GS1_LINTER_INVALID_CSET64_PADDING, "123*=*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12==", GS1_LINTER_INVALID_CSET64_PADDING, "12*==*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "1===", GS1_LINTER_INVALID_CSET64_PADDING, "1*===*");

	UNIT_TEST_FAIL(gs1_lint_cset64, "1234=", GS1_LINTER_INVALID_CSET64_PADDING, "1234*=*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "123==", GS1_LINTER_INVALID_CSET64_PADDING, "123*==*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12===", GS1_LINTER_INVALID_CSET64_PADDING, "12*===*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12====", GS1_LINTER_INVALID_CSET64_PADDING, "12*====*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12=====", GS1_LINTER_INVALID_CSET64_PADDING, "12*=====*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12======", GS1_LINTER_INVALID_CSET64_PADDING, "12*======*");

	UNIT_TEST_FAIL(gs1_lint_cset64, "=23", GS1_LINTER_INVALID_CSET64_CHARACTER, "*=*23");
	UNIT_TEST_FAIL(gs1_lint_cset64, "1=3", GS1_LINTER_INVALID_CSET64_CHARACTER, "1*=*3");
	UNIT_TEST_FAIL(gs1_lint_cset64, "=234", GS1_LINTER_INVALID_CSET64_CHARACTER, "*=*234");
	UNIT_TEST_FAIL(gs1_lint_cset64, "1=34", GS1_LINTER_INVALID_CSET64_CHARACTER, "1*=*34");
	UNIT_TEST_FAIL(gs1_lint_cset64, "12=4", GS1_LINTER_INVALID_CSET64_CHARACTER, "12*=*4");

	UNIT_TEST_PASS(gs1_lint_cset64, "");
	UNIT_TEST_PASS(gs1_lint_cset64, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	UNIT_TEST_PASS(gs1_lint_cset64, "abcdefghijklmnopqrstuvwxyz");
	UNIT_TEST_PASS(gs1_lint_cset64, "0123456789-_");

	UNIT_TEST_FAIL(gs1_lint_cset64, "ABC ", GS1_LINTER_INVALID_CSET64_CHARACTER, "ABC* *");
	UNIT_TEST_FAIL(gs1_lint_cset64, "AB C", GS1_LINTER_INVALID_CSET64_CHARACTER, "AB* *C");
	UNIT_TEST_FAIL(gs1_lint_cset64, " ABC", GS1_LINTER_INVALID_CSET64_CHARACTER, "* *ABC");

	UNIT_TEST_FAIL(gs1_lint_cset64, "\x01", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x01*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x02", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x02*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x03", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x03*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x04", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x04*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x05", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x05*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x06", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x06*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x07", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x07*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x08", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x08*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x09", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x09*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0A", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0A*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0B", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0B*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0C", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0C*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0D", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0D*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0E", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0E*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x0F", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x0F*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x10", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x10*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x11", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x11*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x12", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x12*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x13", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x13*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x14", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x14*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x15", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x15*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x16", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x16*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x17", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x17*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x18", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x18*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x19", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x19*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1A", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1A*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1B", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1B*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1C", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1C*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1D", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1D*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1E", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1E*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x1F", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x1F*");
	UNIT_TEST_FAIL(gs1_lint_cset64, " ",    GS1_LINTER_INVALID_CSET64_CHARACTER, "* *");
	UNIT_TEST_FAIL(gs1_lint_cset64, "!",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*!*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\"",   GS1_LINTER_INVALID_CSET64_CHARACTER, "*\"*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "#",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*#*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "$",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*$*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "%",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*%*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "&",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*&*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "'",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*'*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "(",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*(*");
	UNIT_TEST_FAIL(gs1_lint_cset64, ")",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*)*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "*",    GS1_LINTER_INVALID_CSET64_CHARACTER, "***");
	UNIT_TEST_FAIL(gs1_lint_cset64, "+",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*+*");
	UNIT_TEST_FAIL(gs1_lint_cset64, ",",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*,*");
	UNIT_TEST_PASS(gs1_lint_cset64, "-");
	UNIT_TEST_FAIL(gs1_lint_cset64, ".",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*.*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "/",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*/*");
	UNIT_TEST_PASS(gs1_lint_cset64, "0");
	UNIT_TEST_PASS(gs1_lint_cset64, "1");
	UNIT_TEST_PASS(gs1_lint_cset64, "2");
	UNIT_TEST_PASS(gs1_lint_cset64, "3");
	UNIT_TEST_PASS(gs1_lint_cset64, "4");
	UNIT_TEST_PASS(gs1_lint_cset64, "5");
	UNIT_TEST_PASS(gs1_lint_cset64, "6");
	UNIT_TEST_PASS(gs1_lint_cset64, "7");
	UNIT_TEST_PASS(gs1_lint_cset64, "8");
	UNIT_TEST_PASS(gs1_lint_cset64, "9");
	UNIT_TEST_FAIL(gs1_lint_cset64, ":",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*:*");
	UNIT_TEST_FAIL(gs1_lint_cset64, ";",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*;*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "<",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*<*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "=",    GS1_LINTER_INVALID_CSET64_PADDING, "*=*");  // Bad padding
	UNIT_TEST_FAIL(gs1_lint_cset64, ">",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*>*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "?",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*?*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "@",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*@*");
	UNIT_TEST_PASS(gs1_lint_cset64, "A");
	UNIT_TEST_PASS(gs1_lint_cset64, "B");
	UNIT_TEST_PASS(gs1_lint_cset64, "C");
	UNIT_TEST_PASS(gs1_lint_cset64, "D");
	UNIT_TEST_PASS(gs1_lint_cset64, "E");
	UNIT_TEST_PASS(gs1_lint_cset64, "F");
	UNIT_TEST_PASS(gs1_lint_cset64, "G");
	UNIT_TEST_PASS(gs1_lint_cset64, "H");
	UNIT_TEST_PASS(gs1_lint_cset64, "I");
	UNIT_TEST_PASS(gs1_lint_cset64, "J");
	UNIT_TEST_PASS(gs1_lint_cset64, "K");
	UNIT_TEST_PASS(gs1_lint_cset64, "L");
	UNIT_TEST_PASS(gs1_lint_cset64, "M");
	UNIT_TEST_PASS(gs1_lint_cset64, "N");
	UNIT_TEST_PASS(gs1_lint_cset64, "O");
	UNIT_TEST_PASS(gs1_lint_cset64, "P");
	UNIT_TEST_PASS(gs1_lint_cset64, "Q");
	UNIT_TEST_PASS(gs1_lint_cset64, "R");
	UNIT_TEST_PASS(gs1_lint_cset64, "S");
	UNIT_TEST_PASS(gs1_lint_cset64, "T");
	UNIT_TEST_PASS(gs1_lint_cset64, "U");
	UNIT_TEST_PASS(gs1_lint_cset64, "V");
	UNIT_TEST_PASS(gs1_lint_cset64, "W");
	UNIT_TEST_PASS(gs1_lint_cset64, "X");
	UNIT_TEST_PASS(gs1_lint_cset64, "Y");
	UNIT_TEST_PASS(gs1_lint_cset64, "Z");
	UNIT_TEST_FAIL(gs1_lint_cset64, "[",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*[*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\\",   GS1_LINTER_INVALID_CSET64_CHARACTER, "*\\*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "]",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*]*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "^",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*^*");
	UNIT_TEST_PASS(gs1_lint_cset64, "_");
	UNIT_TEST_FAIL(gs1_lint_cset64, "`",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*`*");
	UNIT_TEST_PASS(gs1_lint_cset64, "a");
	UNIT_TEST_PASS(gs1_lint_cset64, "b");
	UNIT_TEST_PASS(gs1_lint_cset64, "c");
	UNIT_TEST_PASS(gs1_lint_cset64, "d");
	UNIT_TEST_PASS(gs1_lint_cset64, "e");
	UNIT_TEST_PASS(gs1_lint_cset64, "f");
	UNIT_TEST_PASS(gs1_lint_cset64, "g");
	UNIT_TEST_PASS(gs1_lint_cset64, "h");
	UNIT_TEST_PASS(gs1_lint_cset64, "i");
	UNIT_TEST_PASS(gs1_lint_cset64, "j");
	UNIT_TEST_PASS(gs1_lint_cset64, "k");
	UNIT_TEST_PASS(gs1_lint_cset64, "l");
	UNIT_TEST_PASS(gs1_lint_cset64, "m");
	UNIT_TEST_PASS(gs1_lint_cset64, "n");
	UNIT_TEST_PASS(gs1_lint_cset64, "o");
	UNIT_TEST_PASS(gs1_lint_cset64, "p");
	UNIT_TEST_PASS(gs1_lint_cset64, "q");
	UNIT_TEST_PASS(gs1_lint_cset64, "r");
	UNIT_TEST_PASS(gs1_lint_cset64, "s");
	UNIT_TEST_PASS(gs1_lint_cset64, "t");
	UNIT_TEST_PASS(gs1_lint_cset64, "u");
	UNIT_TEST_PASS(gs1_lint_cset64, "v");
	UNIT_TEST_PASS(gs1_lint_cset64, "w");
	UNIT_TEST_PASS(gs1_lint_cset64, "x");
	UNIT_TEST_PASS(gs1_lint_cset64, "y");
	UNIT_TEST_PASS(gs1_lint_cset64, "z");
	UNIT_TEST_FAIL(gs1_lint_cset64, "{",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*{*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "|",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*|*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "}",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*}*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "~",    GS1_LINTER_INVALID_CSET64_CHARACTER, "*~*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x7F", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x7F*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x80", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x80*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x81", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x81*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x82", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x82*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x83", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x83*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x84", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x84*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x85", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x85*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x86", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x86*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x87", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x87*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x88", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x88*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x89", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x89*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8A", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8A*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8B", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8B*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8C", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8C*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8D", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8D*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8E", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8E*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x8F", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x8F*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x90", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x90*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x91", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x91*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x92", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x92*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x93", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x93*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x94", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x94*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x95", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x95*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x96", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x96*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x97", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x97*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x98", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x98*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x99", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x99*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9A", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9A*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9B", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9B*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9C", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9C*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9D", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9D*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9E", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9E*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\x9F", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\x9F*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xA9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xA9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAD", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAD*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xAF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xAF*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xB9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xB9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBD", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBD*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xBF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xBF*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xC9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xC9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCD", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCD*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xCF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xCF*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xD9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xD9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDD", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDD*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xDF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xDF*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xE9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xE9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xEA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xEA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xEB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xEB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xEC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xEC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xED", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xED*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xEE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xEE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xEF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xEF*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF0", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF0*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF1", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF1*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF2", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF2*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF3", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF3*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF4", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF4*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF5", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF5*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF6", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF6*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF7", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF7*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF8", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF8*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xF9", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xF9*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFA", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFA*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFB", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFB*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFC", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFC*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFD", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFD*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFE", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFE*");
	UNIT_TEST_FAIL(gs1_lint_cset64, "\xFF", GS1_LINTER_INVALID_CSET64_CHARACTER, "*\xFF*");

}

#endif  /* UNIT_TESTS */
