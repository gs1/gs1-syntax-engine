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

#include <string.h>

#include "gs1syntaxdictionary.h"


struct name_function_s {
	char *name;
	gs1_linter_t fn;
};

const struct name_function_s name_function_map[] = {
	{ .name = "couponcode",		.fn = gs1_lint_couponcode },
	{ .name = "couponposoffer",	.fn = gs1_lint_couponposoffer },
	{ .name = "cset39",		.fn = gs1_lint_cset39 },
	{ .name = "cset82",		.fn = gs1_lint_cset82 },
	{ .name = "csetnumeric",	.fn = gs1_lint_csetnumeric },
	{ .name = "csum",		.fn = gs1_lint_csum },
	{ .name = "csumalpha",		.fn = gs1_lint_csumalpha },
	{ .name = "hhmm",		.fn = gs1_lint_hhmm },
	{ .name = "iban",		.fn = gs1_lint_iban },
	{ .name = "importeridx",	.fn = gs1_lint_importeridx },
	{ .name = "iso3166",		.fn = gs1_lint_iso3166 },
	{ .name = "iso3166999",		.fn = gs1_lint_iso3166999 },
	{ .name = "iso3166alpha2",	.fn = gs1_lint_iso3166alpha2 },
	{ .name = "iso3166list",	.fn = gs1_lint_iso3166list },
	{ .name = "iso4217",		.fn = gs1_lint_iso4217 },
	{ .name = "key",		.fn = gs1_lint_key },
	{ .name = "latlong",		.fn = gs1_lint_latlong },
	{ .name = "mmoptss",		.fn = gs1_lint_mmoptss },
	{ .name = "nonzero",		.fn = gs1_lint_nonzero },
	{ .name = "nozeroprefix",	.fn = gs1_lint_nozeroprefix },
	{ .name = "pcenc",		.fn = gs1_lint_pcenc },
	{ .name = "pieceoftotal",	.fn = gs1_lint_pieceoftotal },
	{ .name = "winding",		.fn = gs1_lint_winding },
	{ .name = "yesno",		.fn = gs1_lint_yesno },
	{ .name = "yymmd0",		.fn = gs1_lint_yymmd0 },
	{ .name = "yymmdd",		.fn = gs1_lint_yymmdd },
	{ .name = "yymmddhh",		.fn = gs1_lint_yymmddhh },
	{ .name = "zero",		.fn = gs1_lint_zero },
};



/*
 * Return the linter function corresponding to a linter name.
 *
 */
gs1_linter_t gs1_linter_from_name(const char *name) {

	size_t s = 0, e = sizeof(name_function_map) / sizeof(name_function_map[0]);

	while (s < e) {

		size_t m = s + (e - s) / 2;
		int cmp = strcmp(name_function_map[m].name, name);

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

const char *gs1_lint_err_str[__GS1_LINTER_NUM_ERRS] = {
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
	"The component is shorter than the minimum length GS1 Company Prefix.",
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
	"The date is too short for YYMMDD format.",
	"The date is too long for YYMMDD format.",
	"The date with hour is too short for YYMMDDHH format.",
	"The date with hour is too long for YYMMDDHH format.",
	"The hour with minute is too short for HHMM format.",
	"The hour with minute is too long for HHMM format.",
	"The minutes with optional seconds has an incorrect length for either MMSS or MM format.",
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
	"The coupon's expiration date preceed the start date.",
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
	"The concatenated latitude and longitude value must be 20 digits.",
	"The latitude is outside of the range \"0000000000\" to \"1800000000\".",
	"The longitude is outside of the range \"0000000000\" to \"3600000000\".",
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
	TEST_CHECK(gs1_linter_from_name("key") == gs1_lint_key);
	TEST_CHECK(gs1_linter_from_name("dummy") == NULL);
}

#endif  /* UNIT_TESTS */
