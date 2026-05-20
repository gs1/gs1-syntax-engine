/**
 * GS1 Barcode Syntax Dictionary
 *
 * @author Copyright (c) 2022-2026 GS1 AISBL.
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

#include <stddef.h>

int test_gcp_lookup_result = 0;
int test_gcp_lookup_countdown = 0;


/*
 *  Permit sprintf on MacOS clang.
 *  Don't perform code analysis of third-party code that is only used for testing.
 *
 */
#if defined(__clang__)
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#elif defined(_MSC_VER)
#  include <CodeAnalysis/warnings.h>
#  pragma warning(push)
#  pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#endif
#include "acutest.h"
#if defined(__clang__)
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif


void test_lint_couponcode(void);
void test_lint_couponposoffer(void);
void test_lint_cset39(void);
void test_lint_cset64(void);
void test_lint_cset82(void);
void test_lint_csetnumeric(void);
void test_lint_csum(void);
void test_lint_csumalpha(void);
void test_lint_gcppos1(void);
void test_lint_gcppos2(void);
void test_lint_hasnondigit(void);
void test_lint_hh(void);
void test_lint_hhmi(void);
void test_lint_hyphen(void);
void test_lint_iban(void);
void test_lint_importeridx(void);
void test_lint_iso3166(void);
void test_lint_iso3166999(void);
void test_lint_iso3166alpha2(void);
void test_lint_iso4217(void);
void test_lint_iso5218(void);
void test_lint_latitude(void);
void test_lint_longitude(void);
void test_lint_mediatype(void);
void test_lint_mi(void);
void test_lint_nonzero(void);
void test_lint_nozeroprefix(void);
void test_lint_packagetype(void);
void test_lint_pcenc(void);
void test_lint_pieceoftotal(void);
void test_lint_posinseqslash(void);
void test_lint_ss(void);
void test_lint_winding(void);
void test_lint_yesno(void);
void test_lint_yymmd0(void);
void test_lint_yymmdd(void);
void test_lint_yyyymmd0(void);
void test_lint_yyyymmdd(void);
void test_lint_zero(void);

void test_lint__stubs(void);

void test_name_function_map_is_sorted(void);
void test_gs1_linter_from_name(void);
void test_gs1_linter_err_str_en_size(void);


TEST_LIST = {

	{ "lint_couponcode", test_lint_couponcode },
	{ "lint_couponposoffer", test_lint_couponposoffer },
	{ "lint_cset39", test_lint_cset39 },
	{ "lint_cset64", test_lint_cset64 },
	{ "lint_cset82", test_lint_cset82 },
	{ "lint_csetnumeric", test_lint_csetnumeric },
	{ "lint_csum", test_lint_csum },
	{ "lint_csumalpha", test_lint_csumalpha },
	{ "lint_gcppos1", test_lint_gcppos1 },
	{ "lint_gcppos2", test_lint_gcppos2 },
	{ "lint_hasnondigit", test_lint_hasnondigit },
	{ "lint_hh", test_lint_hh },
	{ "lint_hhmi", test_lint_hhmi },
	{ "lint_hyphen", test_lint_hyphen },
	{ "lint_importeridx", test_lint_importeridx },
	{ "lint_nonzero", test_lint_nonzero },
	{ "lint_nozeroprefix", test_lint_nozeroprefix },
	{ "lint_iban", test_lint_iban },
	{ "lint_iso3166", test_lint_iso3166 },
	{ "lint_iso3166999", test_lint_iso3166999 },
	{ "lint_iso3166alpha2", test_lint_iso3166alpha2 },
	{ "lint_iso4217", test_lint_iso4217 },
	{ "lint_iso5218", test_lint_iso5218 },
	{ "lint_latitude", test_lint_latitude },
	{ "lint_longitude", test_lint_longitude },
	{ "lint_mediatype", test_lint_mediatype },
	{ "lint_mi", test_lint_mi },
	{ "lint_packagetype", test_lint_packagetype },
	{ "lint_pcenc", test_lint_pcenc },
	{ "lint_pieceoftotal", test_lint_pieceoftotal },
	{ "lint_posinseqslash", test_lint_posinseqslash },
	{ "lint_ss", test_lint_ss },
	{ "lint_winding", test_lint_winding },
	{ "lint_yesno", test_lint_yesno },
	{ "lint_yymmd0", test_lint_yymmd0 },
	{ "lint_yymmdd", test_lint_yymmdd },
	{ "lint_yyyymmd0", test_lint_yyyymmd0 },
	{ "lint_yyyymmdd", test_lint_yyyymmdd },
	{ "lint_zero", test_lint_zero },

	{ "lint__stubs", test_lint__stubs },

	{ "name_function_map_is_sorted", test_name_function_map_is_sorted },
	{ "gs1_linter_from_name", test_gs1_linter_from_name },
#ifdef GS1_LINTER_ERR_STR_EN
	{ "gs1_linter_err_str_en_size", test_gs1_linter_err_str_en_size },
#endif

	{ NULL, NULL }

};
