/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2025 GS1 AISBL.
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

#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


struct name_function_s {
	char *name;
	gs1_linter_t fn;
};


#define ENT(x) { .name = #x, .fn = gs1_lint_##x }
#define DEP(x)					\
DIAG_PUSH					\
DIAG_DISABLE_DEPRECATED_DECLARATIONS		\
ENT(x)						\
DIAG_POP

/* GCC has flaky support for pragmas within expressions */
#if defined(__GNUC__) && !defined(__clang__)
  #undef DEP
  #define DEP(x) ENT(x)
  DIAG_PUSH
  DIAG_DISABLE_DEPRECATED_DECLARATIONS
#endif

const struct name_function_s name_function_map[] = {
	ENT(couponcode),
	ENT(couponposoffer),
	ENT(cset39),
	ENT(cset64),
	ENT(cset82),
	ENT(csetnumeric),
	ENT(csum),
	ENT(csumalpha),
	ENT(gcppos1),
	ENT(gcppos2),
	ENT(hasnondigit),
	ENT(hh),
	ENT(hhmi),
	DEP(hhmm),
	ENT(hyphen),
	ENT(iban),
	ENT(importeridx),
	ENT(iso3166),
	ENT(iso3166999),
	ENT(iso3166alpha2),
	DEP(iso3166list),
	ENT(iso4217),
	ENT(iso5218),
	DEP(key),
	DEP(keyoff1),
	ENT(latitude),
	ENT(longitude),
	ENT(mediatype),
	ENT(mi),
	DEP(mmoptss),
	ENT(nonzero),
	ENT(nozeroprefix),
	ENT(packagetype),
	ENT(pcenc),
	ENT(pieceoftotal),
	ENT(posinseqslash),
	ENT(ss),
	ENT(winding),
	ENT(yesno),
	ENT(yymmd0),
	ENT(yymmdd),
	DEP(yymmddhh),
	ENT(yyyymmd0),
	ENT(yyyymmdd),
	ENT(zero),
};

/* Flaky GCC */
#if defined(__GNUC__) && !defined(__clang__)
  DIAG_POP
#endif

#undef ENT
#undef DEP


/*
 * Return the linter function corresponding to a linter name.
 *
 */
gs1_linter_t gs1_linter_from_name(const char* const name) {

	size_t s = 0, e = sizeof(name_function_map) / sizeof(name_function_map[0]);

	while (s < e) {

		const size_t m = s + (e - s) / 2;
		const int cmp = strcmp(name_function_map[m].name, name);

		if (cmp == 0)
			return name_function_map[m].fn;
		if (cmp < 0)
			s = m + 1;
		else
			e = m;

	}

	return NULL;

}


/*
 * Example mapping of gs1_lint_err_t entries to friendly strings in the English
 * language.
 *
 * Order matches that of gs1_lint_err_t.
 *
 */
#ifdef GS1_LINTER_ERR_STR_EN

GS1_SYNTAX_DICTIONARY_API const char *gs1_lint_err_str[] = {
	"No issues were detected by the linter.",
	"A non-digit character was found where a digit is expected.",
	"A non-CSET 82 character was found where a CSET 82 character is expected.",
	"A non-CSET 39 character was found where a CSET 39 character is expected.",
	"A non-CSET 32 character was found where a CSET 32 character is expected.",
	"The numeric check digit is incorrect.",
	"The component is too short to perform a numeric check digit calculation.",
	"The alphanumeric check-character pair are incorrect.",
	"The component is too short to perform an alphanumeric check character pair calculation.",
	"The component is too long to perform an alphanumeric check character pair calculation.",
	"The data source for GCP lookups is offline.",
	"",	// Unused
	"The GS1 Company Prefix is invalid.",
	"The Importer Index must be a single character.",
	"The Importer Index is an invalid character.",
	"A non-zero value is required.",
	"A zero is required.",
	"A zero prefix is not permitted.",
	"A \"0\" or \"1\" is required.",
	"The winding direction must be either \"0\", \"1\" or \"9\".",
	"A valid ISO 3166 three-digit country code is required.",
	"A valid ISO 3166 three-digit country code or \"999\" is required.",
	"A valid ISO 3166 two-character country code is required.",
	"A valid ISO 4217 three-digit currency code is required.",
	"The IBAN is too short.",
	"The IBAN contains an invalid character.",
	"The IBAN must start with a valid ISO 3166 two-character country code.",
	"The IBAN is invalid since the check characters are incorrect.",
	"The date is too short.",
	"The date is too long.",
	"",	// Unused
	"",	// Unused
	"",	// Unused
	"The hour with minute is too short for HHMI format.",
	"The hour with minute is too long for HHMI format.",
	"",	// Unused
	"The date contains an illegal month of the year.",
	"The date contains an illegal day of the month.",
	"The time contains an illegal hour.",
	"The time contains an illegal minute.",
	"The time contains an illegal seconds.",
	"The piece with total must have an even length, having equal-length components.",
	"The piece number must not have a value of zero.",
	"The piece total must not have a value of zero.",
	"The piece number must not exceed the piece total.",
	"The input contains an invalid percent hex-encoding \"%hh\" sequence.",
	"The coupon's Format Code is missing.",
	"The coupon's Format Code must be \"0\" or \"1\".",
	"The coupon's Funder VLI is missing.",
	"The coupon's Funder VLI must be \"0\" to \"6\".",
	"The coupon's Funder is shorter than what is indicated by its VLI.",
	"The coupon's Offer Code is shorter than the required six digits.",
	"The coupon's Serial Number VLI is missing.",
	"The coupon's Serial Number is shorter than what is indicated by its VLI.",
	"The coupon's primary GS1 Company Prefix VLI is missing.",
	"The coupon's primary GS1 Company Prefix VLI must be \"0\" to \"6\".",
	"The coupon's primary GS1 Company Prefix is shorter than what is indicated by its VLI.",
	"The coupon's Save Value VLI is missing.",
	"The coupon's Save Value VLI must be \"1\" to \"5\".",
	"The coupon's Save Value is shorter than what is indicated by its VLI.",
	"The coupon's primary purchase Requirement VLI is missing.",
	"The coupon's primary purchase Requirement VLI must be \"1\" to \"5\".",
	"The coupon's primary purchase Requirement is shorter than what is indicated by its VLI.",
	"The coupon's primary purchase Requirement Code is missing.",
	"The coupon's primary purchase Requirement Code must be \"0\" to \"4\" or \"9\".",
	"The coupon's primary purchase Family Code is shorter than the required three digits.",
	"The coupon's Additional Purchase Rules Code is missing.",
	"The coupon's Additional Purchase Rules Code must be \"0\" to \"3\".",
	"The coupon's second purchase Requirement VLI is missing.",
	"The coupon's second purchase Requirement VLI must be \"1\" to \"5\".",
	"The coupon's second purchase Requirement is shorter than what is indicated by its VLI.",
	"The coupon's second purchase Requirement Code is missing.",
	"The coupon's second purchase Requirement Code must be \"0\" to \"4\" or \"9\".",
	"The coupon's second purchase Family Code is shorter than the required three digits.",
	"The coupon's second purchase GS1 Company Prefix VLI is missing.",
	"The coupon's second purchase GS1 Company Prefix VLI must be \"0\" to \"6\" or \"9\".",
	"The coupon's second purchase GS1 Company Prefix is shorter than what is indicated by its VLI.",
	"The coupon's third purchase Requirement VLI is missing.",
	"The coupon's third purchase Requirement VLI must be \"1\" to \"5\".",
	"The coupon's third purchase Requirement is shorter than what is indicated by its VLI.",
	"The coupon's third purchase Requirement Code is missing.",
	"The coupon's third purchase Requirement Code must be \"0\" to \"4\" or \"9\".",
	"The coupon's third purchase Family Code is shorter than the required three digits.",
	"The coupon's third purchase GS1 Company Prefix VLI is missing.",
	"The coupon's third purchase GS1 Company Prefix VLI must be \"0\" to \"6\" or \"9\".",
	"The coupon's third purchase GS1 Company Prefix is shorter than what is indicated by its VLI.",
	"The coupon's expiration date is too short for YYMMDD format.",
	"The coupon's expiration date is invalid.",
	"The coupon's start date is too short to YYMMDD format.",
	"The coupon's start date is invalid.",
	"The coupon's expiration date precede the start date.",
	"The coupon's Retailer GCP/GLN VLI is missing.",
	"The coupon's Retailer GCP/GLN VLI must be \"1\" to \"7\".",
	"The coupon's Retailer GCP/GLN is shorter than what is indicated by its VLI.",
	"The coupon's Save Value Code is missing.",
	"The coupon's Save Value Code must be \"0\", \"1\", \"2\", \"5\" or \"6\".",
	"The coupon's Save Value Applies to Item is missing.",
	"The coupon's Save Value Applies to Item must be \"0\" to \"2\".",
	"The coupon's Store Coupon Flag is missing.",
	"The coupon's Don't Multiply Flag is missing.",
	"The coupon's Don't Multiply Flag must be \"0\" or \"1\".",
	"The coupon contains excess data after the recognised optional fields.",
	"",	// Unused
	"The latitude is outside of the range \"0000000000\" to \"1800000000\".",
	"The longitude is outside of the range \"0000000000\" to \"3600000000\".",
	"A valid AIDC media type is required.",
	"The latitude must be 10 digits.",
	"The longitude must be 10 digits.",
	"A non-CSET 64 character was found where a CSET 64 character is expected.",
	"Incorrect number of CSET 64 pad characters.",
	"Only hyphens are permitted.",
	"A valid ISO/IEC 5218 biological sex code required.",
	"The data must have the format \"<pos>/<end>\".",
	"The position number must not exceed the end number.",
	"A non-digit character is required.",
	"The hour is too short for HH format.",
	"The hour is too long for HH format.",
	"The minute is too short for MI format.",
	"The minute is too long for MI format.",
	"The second is too short for SS format.",
	"The second is too long for SS format.",
	"A valid PackageTypeCode is required.",
	"The component is shorter than the minimum length GS1 Company Prefix.",
	"The IBAN is too long.",
};

#endif  /* GS1_LINTER_ERR_STR_EN */



#ifdef UNIT_TESTS

#define TEST_NO_MAIN
#include "acutest.h"

void test_name_function_map_is_sorted(void)
{

	size_t i;

	for (i = 1; i < sizeof(name_function_map) / sizeof(name_function_map[0]); i++) {
		TEST_CHECK(strcmp(name_function_map[i].name, name_function_map[i-1].name) > 0);
	}

}

void test_gs1_linter_from_name(void)
{
	TEST_CHECK(gs1_linter_from_name("gcppos1") == gs1_lint_gcppos1);
	TEST_CHECK(gs1_linter_from_name("dummy") == NULL);

DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS
	TEST_CHECK(gs1_linter_from_name("key") == gs1_lint_key);
DIAG_POP

}


#ifdef GS1_LINTER_ERR_STR_EN
void test_gs1_linter_err_str_en_size(void)
{
	TEST_CHECK(sizeof(gs1_lint_err_str)/sizeof(gs1_lint_err_str[0]) == __GS1_LINTER_NUM_ERRS);
}
#endif

#endif  /* UNIT_TESTS */
