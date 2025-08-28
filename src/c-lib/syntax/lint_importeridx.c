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
 * @file lint_importeridx.c
 *
 * @brief The `importeridx` linter ensures that the given data is a valid
 * Importer Index, as included in a Facility ID or Machine ID used in the EU
 * 2018/574 system.
 *
 * @remark The Facility ID and Machine ID data elements used in the EU 2018/574
 * system are described in the [GS1 General
 * Specification](https://www.gs1.org/genspecs) section "European Regulation
 * 2018/574, traceability of tobacco products".
 *
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component is a valid Importer Index.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER if the data is not a
 *         valid Importer Index character.
 * @return #GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER if the data isn't a
 *         single character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_importeridx(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	/*
	 * Importer index characters: -0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
	 *
	 */
	static const uint64_t importeridx_bitfield[] = {
		0x000000000004ffc0,		// "-", 0-9
		0x7fffffe17fffffe0,		// A-Z, "_", a-z
		0x0000000000000000,
		0x0000000000000000
	};

	int valid;

	assert(data);

	/*
	 * Data must be a single character.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data[0] == '\0' || data[1] != '\0')) {
		size_t len = data[0] == '\0' ? 0 : strlen(data);
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER,
			0,
			len
		);
	}

	/*
	 * Validate character using bitfield lookup
	 *
	 */
	GS1_LINTER_BITFIELD_LOOKUP((unsigned char)data[0], importeridx_bitfield, valid);
	if (GS1_LINTER_UNLIKELY(!valid))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER,
			0,
			1
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_importeridx(void)
{

	UNIT_TEST_FAIL(gs1_lint_importeridx, "",   GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER, "**");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "AA", GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER, "*AA*");

	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x01", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x01*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x02", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x02*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x03", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x03*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x04", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x04*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x05", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x05*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x06", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x06*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x07", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x07*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x08", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x08*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x09", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x09*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0A", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0A*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0B", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0B*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0C", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0C*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0D", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0D*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0E", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0E*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x0F", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x0F*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x10", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x10*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x11", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x11*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x12", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x12*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x13", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x13*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x14", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x14*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x15", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x15*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x16", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x16*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x17", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x17*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x18", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x18*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x19", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x19*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1A", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1A*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1B", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1B*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1C", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1C*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1D", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1D*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1E", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1E*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x1F", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x1F*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, " ",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "* *");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "!",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*!*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\"",   GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\"*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "#",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*#*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "$",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*$*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "%",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*%*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "&",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*&*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "'",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*'*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "(",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*(*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ")",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*)*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "*",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "***");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "+",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*+*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ",",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*,*");
	UNIT_TEST_PASS(gs1_lint_importeridx, "-");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ".",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*.*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "/",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*/*");
	UNIT_TEST_PASS(gs1_lint_importeridx, "0");
	UNIT_TEST_PASS(gs1_lint_importeridx, "1");
	UNIT_TEST_PASS(gs1_lint_importeridx, "2");
	UNIT_TEST_PASS(gs1_lint_importeridx, "3");
	UNIT_TEST_PASS(gs1_lint_importeridx, "4");
	UNIT_TEST_PASS(gs1_lint_importeridx, "5");
	UNIT_TEST_PASS(gs1_lint_importeridx, "6");
	UNIT_TEST_PASS(gs1_lint_importeridx, "7");
	UNIT_TEST_PASS(gs1_lint_importeridx, "8");
	UNIT_TEST_PASS(gs1_lint_importeridx, "9");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ":",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*:*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ";",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*;*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "<",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*<*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "=",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*=*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, ">",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*>*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "?",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*?*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "@",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*@*");
	UNIT_TEST_PASS(gs1_lint_importeridx, "A");
	UNIT_TEST_PASS(gs1_lint_importeridx, "B");
	UNIT_TEST_PASS(gs1_lint_importeridx, "C");
	UNIT_TEST_PASS(gs1_lint_importeridx, "D");
	UNIT_TEST_PASS(gs1_lint_importeridx, "E");
	UNIT_TEST_PASS(gs1_lint_importeridx, "F");
	UNIT_TEST_PASS(gs1_lint_importeridx, "G");
	UNIT_TEST_PASS(gs1_lint_importeridx, "H");
	UNIT_TEST_PASS(gs1_lint_importeridx, "I");
	UNIT_TEST_PASS(gs1_lint_importeridx, "J");
	UNIT_TEST_PASS(gs1_lint_importeridx, "K");
	UNIT_TEST_PASS(gs1_lint_importeridx, "L");
	UNIT_TEST_PASS(gs1_lint_importeridx, "M");
	UNIT_TEST_PASS(gs1_lint_importeridx, "N");
	UNIT_TEST_PASS(gs1_lint_importeridx, "O");
	UNIT_TEST_PASS(gs1_lint_importeridx, "P");
	UNIT_TEST_PASS(gs1_lint_importeridx, "Q");
	UNIT_TEST_PASS(gs1_lint_importeridx, "R");
	UNIT_TEST_PASS(gs1_lint_importeridx, "S");
	UNIT_TEST_PASS(gs1_lint_importeridx, "T");
	UNIT_TEST_PASS(gs1_lint_importeridx, "U");
	UNIT_TEST_PASS(gs1_lint_importeridx, "V");
	UNIT_TEST_PASS(gs1_lint_importeridx, "W");
	UNIT_TEST_PASS(gs1_lint_importeridx, "X");
	UNIT_TEST_PASS(gs1_lint_importeridx, "Y");
	UNIT_TEST_PASS(gs1_lint_importeridx, "Z");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "[",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*[*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\\",   GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\\*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "]",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*]*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "^",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*^*");
	UNIT_TEST_PASS(gs1_lint_importeridx, "_");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "`",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*`*");
	UNIT_TEST_PASS(gs1_lint_importeridx, "a");
	UNIT_TEST_PASS(gs1_lint_importeridx, "b");
	UNIT_TEST_PASS(gs1_lint_importeridx, "c");
	UNIT_TEST_PASS(gs1_lint_importeridx, "d");
	UNIT_TEST_PASS(gs1_lint_importeridx, "e");
	UNIT_TEST_PASS(gs1_lint_importeridx, "f");
	UNIT_TEST_PASS(gs1_lint_importeridx, "g");
	UNIT_TEST_PASS(gs1_lint_importeridx, "h");
	UNIT_TEST_PASS(gs1_lint_importeridx, "i");
	UNIT_TEST_PASS(gs1_lint_importeridx, "j");
	UNIT_TEST_PASS(gs1_lint_importeridx, "k");
	UNIT_TEST_PASS(gs1_lint_importeridx, "l");
	UNIT_TEST_PASS(gs1_lint_importeridx, "m");
	UNIT_TEST_PASS(gs1_lint_importeridx, "n");
	UNIT_TEST_PASS(gs1_lint_importeridx, "o");
	UNIT_TEST_PASS(gs1_lint_importeridx, "p");
	UNIT_TEST_PASS(gs1_lint_importeridx, "q");
	UNIT_TEST_PASS(gs1_lint_importeridx, "r");
	UNIT_TEST_PASS(gs1_lint_importeridx, "s");
	UNIT_TEST_PASS(gs1_lint_importeridx, "t");
	UNIT_TEST_PASS(gs1_lint_importeridx, "u");
	UNIT_TEST_PASS(gs1_lint_importeridx, "v");
	UNIT_TEST_PASS(gs1_lint_importeridx, "w");
	UNIT_TEST_PASS(gs1_lint_importeridx, "x");
	UNIT_TEST_PASS(gs1_lint_importeridx, "y");
	UNIT_TEST_PASS(gs1_lint_importeridx, "z");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "{",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*{*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "|",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*|*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "}",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*}*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "~",    GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*~*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x7F", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x7F*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x80", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x80*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x81", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x81*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x82", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x82*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x83", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x83*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x84", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x84*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x85", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x85*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x86", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x86*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x87", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x87*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x88", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x88*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x89", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x89*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8A", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8A*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8B", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8B*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8C", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8C*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8D", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8D*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8E", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8E*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x8F", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x8F*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x90", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x90*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x91", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x91*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x92", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x92*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x93", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x93*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x94", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x94*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x95", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x95*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x96", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x96*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x97", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x97*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x98", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x98*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x99", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x99*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9A", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9A*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9B", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9B*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9C", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9C*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9D", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9D*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9E", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9E*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\x9F", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\x9F*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xA9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xA9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAD", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAD*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xAF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xAF*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xB9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xB9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBD", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBD*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xBF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xBF*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xC9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xC9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCD", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCD*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xCF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xCF*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xD9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xD9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDD", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDD*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xDF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xDF*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xE9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xE9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xEA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xEA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xEB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xEB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xEC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xEC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xED", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xED*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xEE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xEE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xEF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xEF*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF0", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF0*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF1", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF1*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF2", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF2*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF3", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF3*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF4", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF4*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF5", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF5*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF6", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF6*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF7", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF7*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF8", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF8*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xF9", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xF9*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFA", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFA*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFB", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFB*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFC", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFC*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFD", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFD*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFE", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFE*");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "\xFF", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*\xFF*");

}

#endif  /* UNIT_TESTS */
