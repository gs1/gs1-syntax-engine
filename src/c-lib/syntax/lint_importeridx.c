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

	size_t len;

	/*
	 * All valid Importer Index characters.
	 *
	 */
	static const char* const importeridx =
		"-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

	assert(data);

	len = strlen(data);

	/*
	 * Data must be a single character.
	 *
	 */
	if (len != 1)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER,
			0,
			len
		);

	/*
	 * Any character outside of the valid Importer Index character list is
	 * illegal.
	 *
	 */
	if (strspn(data, importeridx) != len)
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

	UNIT_TEST_PASS(gs1_lint_importeridx, "-");
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
	UNIT_TEST_PASS(gs1_lint_importeridx, "_");
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

	UNIT_TEST_FAIL(gs1_lint_importeridx, " ", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "* *");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "@", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*@*");

}

#endif  /* UNIT_TESTS */
