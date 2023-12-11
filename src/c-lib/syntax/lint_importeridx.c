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
	if (len != 1) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER;
	}

	/*
	 * Any character outside of the valid Importer Index character list is
	 * illegal.
	 *
	 */
	if (strspn(data, importeridx) != len) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 1;
		return GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_importeridx(void)
{

	UNIT_TEST_FAIL(gs1_lint_importeridx, "",   GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER, "**");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "AA", GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER, "*AA*");

	UNIT_TEST_PASS(gs1_lint_importeridx, "-");
	UNIT_TEST_PASS(gs1_lint_importeridx, "0");
	UNIT_TEST_PASS(gs1_lint_importeridx, "9");
	UNIT_TEST_PASS(gs1_lint_importeridx, "A");
	UNIT_TEST_PASS(gs1_lint_importeridx, "Z");
	UNIT_TEST_PASS(gs1_lint_importeridx, "_");
	UNIT_TEST_PASS(gs1_lint_importeridx, "a");
	UNIT_TEST_PASS(gs1_lint_importeridx, "z");

	UNIT_TEST_FAIL(gs1_lint_importeridx, " ", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "* *");
	UNIT_TEST_FAIL(gs1_lint_importeridx, "@", GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER, "*@*");

}

#endif  /* UNIT_TESTS */
