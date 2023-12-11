/*
 * GS1 Syntax Dictionary. Copyright (c) 2023 GS1 AISBL.
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
 * @file lint_mediatype.c
 *
 * @brief The `mediatype` linter ensures that the data represents a valid AIDC
 * media type.
 *
 * @remark The two-digit AIDC media type codes are defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) table "AIDC media type
 * values".
 *
 */


#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/*
 * Include a header containing a replacement lookup function, if we are told
 * to.
 *
 */
#ifdef GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP_H
#define xstr(s) str(s)
#define str(s) #s
#include xstr(GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP_H)
#endif


/**
 * Used to validate that an AI component is a valid AIDC media type.
 *
 * @note The default lookup function provided by this linter is a binary search
 *       over a static list this is maintained in this file.
 * @note To enable this linter to hook into an alternative AIDC media type
 *       lookup function (provided by the user) the
 *       GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP_H macro may be set to the name of a
 *       header file to be included that defines a custom
 *       `GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP` macro.
 * @note If provided, the GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP macro shall invoke
 *       whatever functionality is available in the user-provided lookup
 *       function, then using the result must assign to a locally-scoped
 *       variable as follows:
 *         - `valid`: Set to 1 if the lookup was successful. Otherwise 0.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_MEDIA_TYPE if the data is not a num-3 country code.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mediatype(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	/*
	 * Allow for a custom replacement of the lookup code to be provided.
	 *
	 */
#ifdef GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP
#define GS1_LINTER_MEDIA_TYPE_LOOKUP(cc) GS1_LINTER_CUSTOM_MEDIA_TYPE_LOOKUP(cc)
#else

	/*
	 *  Set of valid AIDC media type values
	 *
	 *  MAINTENANCE NOTE:
	 *
	 *  Updates to the AIDC media type list shall be announced by GSCN
	 *
	 */
	static const char mediatypes[][3] = {
		/* 00		Not used */
		"01", "02", "03", "04", "05", "06", "07", "08", "09", "10",	/* ICCBBA assignments */
		/* 11-29	Reserved for future assignment by ICCBBA */
		/* 30-59	Reserved for future assignment by GS1 */
		/* 60-79	Reserved for future assignment by ICCBBA or GS1 */
		"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",	/* ICCBBA local / national use */
		"90", "91", "92", "93", "94", "95", "96", "97", "98", "99",	/* ICCBBA local / national use */
	};

	/*
	 *  Binary search over the above list.
	 *
	 */
/// \cond
#define GS1_LINTER_MEDIA_TYPE_LOOKUP(cc) do {				\
	size_t s = 0;							\
	size_t e = sizeof(mediatypes) / sizeof(mediatypes[0]);		\
	while (s < e) {							\
		const size_t m = s + (e - s) / 2;			\
		const int cmp = strcmp(mediatypes[m], cc);		\
		if (cmp < 0)						\
			s = m + 1;					\
		else if (cmp > 0)					\
			e = m;						\
		else {							\
			valid = 1;					\
			break;						\
		}							\
	}								\
} while (0)
/// \endcond

#endif

	int valid = 0;

	assert(data);

	/*
	 * Ensure that the data is in the list.
	 *
	 */
	GS1_LINTER_MEDIA_TYPE_LOOKUP(data);
	if (valid)
		return GS1_LINTER_OK;

	/*
	 * If not valid then indicate an error.
	 *
	 */
	if (err_pos) *err_pos = 0;
	if (err_len) *err_len = strlen(data);
	return GS1_LINTER_INVALID_MEDIA_TYPE;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_mediatype(void)
{

	UNIT_TEST_FAIL(gs1_lint_mediatype, "", GS1_LINTER_INVALID_MEDIA_TYPE, "**");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "0", GS1_LINTER_INVALID_MEDIA_TYPE, "*0*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "00", GS1_LINTER_INVALID_MEDIA_TYPE, "*00*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "000", GS1_LINTER_INVALID_MEDIA_TYPE, "*000*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "_01", GS1_LINTER_INVALID_MEDIA_TYPE, "*_01*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "01_", GS1_LINTER_INVALID_MEDIA_TYPE, "*01_*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "AA", GS1_LINTER_INVALID_MEDIA_TYPE, "*AA*");

	UNIT_TEST_FAIL(gs1_lint_mediatype, "00", GS1_LINTER_INVALID_MEDIA_TYPE, "*00*");
	UNIT_TEST_PASS(gs1_lint_mediatype, "01");
	UNIT_TEST_PASS(gs1_lint_mediatype, "02");
	UNIT_TEST_PASS(gs1_lint_mediatype, "03");
	UNIT_TEST_PASS(gs1_lint_mediatype, "04");
	UNIT_TEST_PASS(gs1_lint_mediatype, "05");
	UNIT_TEST_PASS(gs1_lint_mediatype, "06");
	UNIT_TEST_PASS(gs1_lint_mediatype, "07");
	UNIT_TEST_PASS(gs1_lint_mediatype, "08");
	UNIT_TEST_PASS(gs1_lint_mediatype, "09");
	UNIT_TEST_PASS(gs1_lint_mediatype, "10");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "11", GS1_LINTER_INVALID_MEDIA_TYPE, "*11*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "12", GS1_LINTER_INVALID_MEDIA_TYPE, "*12*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "13", GS1_LINTER_INVALID_MEDIA_TYPE, "*13*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "14", GS1_LINTER_INVALID_MEDIA_TYPE, "*14*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "15", GS1_LINTER_INVALID_MEDIA_TYPE, "*15*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "16", GS1_LINTER_INVALID_MEDIA_TYPE, "*16*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "17", GS1_LINTER_INVALID_MEDIA_TYPE, "*17*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "18", GS1_LINTER_INVALID_MEDIA_TYPE, "*18*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "19", GS1_LINTER_INVALID_MEDIA_TYPE, "*19*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "20", GS1_LINTER_INVALID_MEDIA_TYPE, "*20*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "21", GS1_LINTER_INVALID_MEDIA_TYPE, "*21*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "22", GS1_LINTER_INVALID_MEDIA_TYPE, "*22*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "23", GS1_LINTER_INVALID_MEDIA_TYPE, "*23*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "24", GS1_LINTER_INVALID_MEDIA_TYPE, "*24*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "25", GS1_LINTER_INVALID_MEDIA_TYPE, "*25*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "26", GS1_LINTER_INVALID_MEDIA_TYPE, "*26*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "27", GS1_LINTER_INVALID_MEDIA_TYPE, "*27*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "28", GS1_LINTER_INVALID_MEDIA_TYPE, "*28*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "29", GS1_LINTER_INVALID_MEDIA_TYPE, "*29*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "30", GS1_LINTER_INVALID_MEDIA_TYPE, "*30*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "31", GS1_LINTER_INVALID_MEDIA_TYPE, "*31*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "32", GS1_LINTER_INVALID_MEDIA_TYPE, "*32*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "33", GS1_LINTER_INVALID_MEDIA_TYPE, "*33*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "34", GS1_LINTER_INVALID_MEDIA_TYPE, "*34*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "35", GS1_LINTER_INVALID_MEDIA_TYPE, "*35*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "36", GS1_LINTER_INVALID_MEDIA_TYPE, "*36*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "37", GS1_LINTER_INVALID_MEDIA_TYPE, "*37*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "38", GS1_LINTER_INVALID_MEDIA_TYPE, "*38*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "39", GS1_LINTER_INVALID_MEDIA_TYPE, "*39*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "40", GS1_LINTER_INVALID_MEDIA_TYPE, "*40*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "41", GS1_LINTER_INVALID_MEDIA_TYPE, "*41*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "42", GS1_LINTER_INVALID_MEDIA_TYPE, "*42*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "43", GS1_LINTER_INVALID_MEDIA_TYPE, "*43*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "44", GS1_LINTER_INVALID_MEDIA_TYPE, "*44*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "45", GS1_LINTER_INVALID_MEDIA_TYPE, "*45*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "46", GS1_LINTER_INVALID_MEDIA_TYPE, "*46*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "47", GS1_LINTER_INVALID_MEDIA_TYPE, "*47*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "48", GS1_LINTER_INVALID_MEDIA_TYPE, "*48*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "49", GS1_LINTER_INVALID_MEDIA_TYPE, "*49*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "50", GS1_LINTER_INVALID_MEDIA_TYPE, "*50*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "51", GS1_LINTER_INVALID_MEDIA_TYPE, "*51*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "52", GS1_LINTER_INVALID_MEDIA_TYPE, "*52*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "53", GS1_LINTER_INVALID_MEDIA_TYPE, "*53*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "54", GS1_LINTER_INVALID_MEDIA_TYPE, "*54*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "55", GS1_LINTER_INVALID_MEDIA_TYPE, "*55*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "56", GS1_LINTER_INVALID_MEDIA_TYPE, "*56*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "57", GS1_LINTER_INVALID_MEDIA_TYPE, "*57*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "58", GS1_LINTER_INVALID_MEDIA_TYPE, "*58*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "59", GS1_LINTER_INVALID_MEDIA_TYPE, "*59*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "60", GS1_LINTER_INVALID_MEDIA_TYPE, "*60*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "61", GS1_LINTER_INVALID_MEDIA_TYPE, "*61*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "62", GS1_LINTER_INVALID_MEDIA_TYPE, "*62*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "63", GS1_LINTER_INVALID_MEDIA_TYPE, "*63*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "64", GS1_LINTER_INVALID_MEDIA_TYPE, "*64*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "65", GS1_LINTER_INVALID_MEDIA_TYPE, "*65*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "66", GS1_LINTER_INVALID_MEDIA_TYPE, "*66*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "67", GS1_LINTER_INVALID_MEDIA_TYPE, "*67*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "68", GS1_LINTER_INVALID_MEDIA_TYPE, "*68*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "69", GS1_LINTER_INVALID_MEDIA_TYPE, "*69*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "70", GS1_LINTER_INVALID_MEDIA_TYPE, "*70*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "71", GS1_LINTER_INVALID_MEDIA_TYPE, "*71*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "72", GS1_LINTER_INVALID_MEDIA_TYPE, "*72*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "73", GS1_LINTER_INVALID_MEDIA_TYPE, "*73*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "74", GS1_LINTER_INVALID_MEDIA_TYPE, "*74*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "75", GS1_LINTER_INVALID_MEDIA_TYPE, "*75*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "76", GS1_LINTER_INVALID_MEDIA_TYPE, "*76*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "77", GS1_LINTER_INVALID_MEDIA_TYPE, "*77*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "78", GS1_LINTER_INVALID_MEDIA_TYPE, "*78*");
	UNIT_TEST_FAIL(gs1_lint_mediatype, "79", GS1_LINTER_INVALID_MEDIA_TYPE, "*79*");
	UNIT_TEST_PASS(gs1_lint_mediatype, "80");
	UNIT_TEST_PASS(gs1_lint_mediatype, "81");
	UNIT_TEST_PASS(gs1_lint_mediatype, "82");
	UNIT_TEST_PASS(gs1_lint_mediatype, "83");
	UNIT_TEST_PASS(gs1_lint_mediatype, "84");
	UNIT_TEST_PASS(gs1_lint_mediatype, "85");
	UNIT_TEST_PASS(gs1_lint_mediatype, "86");
	UNIT_TEST_PASS(gs1_lint_mediatype, "87");
	UNIT_TEST_PASS(gs1_lint_mediatype, "88");
	UNIT_TEST_PASS(gs1_lint_mediatype, "89");
	UNIT_TEST_PASS(gs1_lint_mediatype, "90");
	UNIT_TEST_PASS(gs1_lint_mediatype, "91");
	UNIT_TEST_PASS(gs1_lint_mediatype, "92");
	UNIT_TEST_PASS(gs1_lint_mediatype, "93");
	UNIT_TEST_PASS(gs1_lint_mediatype, "94");
	UNIT_TEST_PASS(gs1_lint_mediatype, "95");
	UNIT_TEST_PASS(gs1_lint_mediatype, "96");
	UNIT_TEST_PASS(gs1_lint_mediatype, "97");
	UNIT_TEST_PASS(gs1_lint_mediatype, "98");
	UNIT_TEST_PASS(gs1_lint_mediatype, "99");

}

#endif  /* UNIT_TESTS */
