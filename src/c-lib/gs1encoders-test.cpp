/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2026 GS1 AISBL.
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

#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>

/*
 *  Don't flag warnings in third-party test harness code.
 */
#if defined(__clang__)
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wclobbered"
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

#include "gs1encoders.hpp"


/* ========================================================================
 *  Construction and RAII
 * ======================================================================== */

static void test_default_construct(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(!gs.version().empty());
	TEST_CHECK(!gs.init_fallback_warning().has_value());
}

static void test_construct_with_empty_opts(void) {
	gs1encoders::GS1Encoder gs(gs1encoders::InitOpts{});
	TEST_CHECK(!gs.version().empty());
}

static void test_construct_with_missing_syndict_throws(void) {
	bool threw = false;
	try {
		gs1encoders::GS1Encoder gs(
			gs1encoders::InitOpts{}.syntax_dictionary(
				"/nonexistent/path/dict.txt"));
	} catch (const gs1encoders::GS1EncoderGeneralException &) {
		threw = true;
	}
	TEST_CHECK(threw);
}

static void test_construct_with_fallback_records_warning(void) {
	gs1encoders::GS1Encoder gs(
		gs1encoders::InitOpts{}
			.syntax_dictionary("/nonexistent/path/dict.txt")
			.fallback_on_syndict_error(true));
	auto warning = gs.init_fallback_warning();
	TEST_CHECK(warning.has_value());
	TEST_CHECK(!warning->empty());
}


/* ========================================================================
 *  Move semantics
 * ======================================================================== */

static void test_move_constructor(void) {
	gs1encoders::GS1Encoder a;
	a.set_ai_data_str("(01)09521234543213");
	gs1encoders::GS1Encoder b(std::move(a));
	TEST_CHECK(b.ai_data_str() == "(01)09521234543213");
}

static void test_move_assignment(void) {
	gs1encoders::GS1Encoder a;
	a.set_ai_data_str("(01)09521234543213");
	gs1encoders::GS1Encoder b;
	b = std::move(a);
	TEST_CHECK(b.ai_data_str() == "(01)09521234543213");
}


/* ========================================================================
 *  Library accessors
 * ======================================================================== */

static void test_version_nonempty(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(!gs.version().empty());
}

static void test_max_data_str_length_positive(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.max_data_str_length() > 0);
}


/* ========================================================================
 *  Boolean configuration getters/setters
 * ======================================================================== */

static void test_add_check_digit_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.add_check_digit() == false);
	gs.set_add_check_digit(true);
	TEST_CHECK(gs.add_check_digit() == true);
	gs.set_add_check_digit(false);
	TEST_CHECK(gs.add_check_digit() == false);
}

static void test_permit_unknown_ais_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.permit_unknown_ais() == false);
	gs.set_permit_unknown_ais(true);
	TEST_CHECK(gs.permit_unknown_ais() == true);
}

static void test_include_data_titles_in_hri_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.include_data_titles_in_hri() == false);
	gs.set_include_data_titles_in_hri(true);
	TEST_CHECK(gs.include_data_titles_in_hri() == true);
}


/* ========================================================================
 *  Symbology
 * ======================================================================== */

static void test_sym_default_is_none(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.sym() == gs1encoders::Symbology::None);
}

static void test_set_sym(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_sym(gs1encoders::Symbology::QR);
	TEST_CHECK(gs.sym() == gs1encoders::Symbology::QR);
}

static void test_set_sym_invalid_throws(void) {
	gs1encoders::GS1Encoder gs;
	bool threw = false;
	try {
		gs.set_sym(gs1encoders::Symbology::NumSyms);
	} catch (const gs1encoders::GS1EncoderParameterException &) {
		threw = true;
	}
	TEST_CHECK(threw);
}


/* ========================================================================
 *  Validation table
 * ======================================================================== */

static void test_validation_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	bool original = gs.validation_enabled(
		gs1encoders::Validation::RequisiteAIs);
	gs.set_validation_enabled(
		gs1encoders::Validation::RequisiteAIs, !original);
	TEST_CHECK(gs.validation_enabled(
		gs1encoders::Validation::RequisiteAIs) == !original);
	gs.set_validation_enabled(
		gs1encoders::Validation::RequisiteAIs, original);
}


/* ========================================================================
 *  AI data input
 * ======================================================================== */

static void test_set_ai_data_str_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_ai_data_str("(01)09521234543213(99)TESTING123");
	TEST_CHECK(gs.ai_data_str() ==
	           "(01)09521234543213(99)TESTING123");
}

static void test_set_ai_data_str_invalid_throws(void) {
	gs1encoders::GS1Encoder gs;
	bool threw = false;
	try {
		gs.set_ai_data_str("(99)\xFF\xFF");  // Bad cset
	} catch (const gs1encoders::GS1EncoderParameterException &e) {
		TEST_CHECK(std::string(e.what()).length() > 0);
		threw = true;
	}
	TEST_CHECK(threw);
}

static void test_set_data_str_dl_uri_round_trip(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_data_str(
		"https://example.com/01/09521234543213?99=TESTING123");
	TEST_CHECK(gs.ai_data_str() ==
	           "(01)09521234543213(99)TESTING123");
}


/* ========================================================================
 *  GS1 Digital Link URI
 * ======================================================================== */

static void test_get_dl_uri(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_ai_data_str("(01)09521234543213(99)TESTING123");
	auto uri = gs.get_dl_uri("https://example.com");
	TEST_CHECK(uri.find("https://example.com") == 0);
	TEST_CHECK(uri.find("/01/09521234543213") != std::string::npos);
}

static void test_get_dl_uri_canonical_stem(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_ai_data_str("(01)09521234543213");
	auto uri = gs.get_dl_uri("");
	TEST_CHECK(uri.find("id.gs1.org") != std::string::npos);
}


/* ========================================================================
 *  HRI
 * ======================================================================== */

static void test_hri_lines(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_ai_data_str("(01)09521234543213(99)TESTING123");
	auto lines = gs.hri();
	TEST_CHECK(lines.size() == 2);
	TEST_CHECK(lines[0] == "(01) 09521234543213");
	TEST_CHECK(lines[1] == "(99) TESTING123");
}

static void test_hri_empty_when_no_data(void) {
	gs1encoders::GS1Encoder gs;
	TEST_CHECK(gs.hri().empty());
}


/* ========================================================================
 *  Scan data
 * ======================================================================== */

static void test_set_scan_data(void) {
	gs1encoders::GS1Encoder gs;
	gs.set_scan_data("]Q3011231231231233310ABC123" "\x1D" "99TESTING");
	TEST_CHECK(gs.ai_data_str() ==
	           "(01)12312312312333(10)ABC123(99)TESTING");
}

static void test_set_scan_data_invalid_throws(void) {
	gs1encoders::GS1Encoder gs;
	bool threw = false;
	try {
		gs.set_scan_data("garbage");
	} catch (const gs1encoders::GS1EncoderScanDataException &) {
		threw = true;
	}
	TEST_CHECK(threw);
}


/* ========================================================================
 *  Test list
 * ======================================================================== */

TEST_LIST = {
	/* Construction and RAII */
	{ "default_construct",                  test_default_construct },
	{ "construct_with_empty_opts",          test_construct_with_empty_opts },
	{ "construct_with_missing_syndict_throws",
	                                        test_construct_with_missing_syndict_throws },
	{ "construct_with_fallback_records_warning",
	                                        test_construct_with_fallback_records_warning },

	/* Move semantics */
	{ "move_constructor",                   test_move_constructor },
	{ "move_assignment",                    test_move_assignment },

	/* Library accessors */
	{ "version_nonempty",                   test_version_nonempty },
	{ "max_data_str_length_positive",       test_max_data_str_length_positive },

	/* Boolean configuration */
	{ "add_check_digit_round_trip",         test_add_check_digit_round_trip },
	{ "permit_unknown_ais_round_trip",      test_permit_unknown_ais_round_trip },
	{ "include_data_titles_in_hri_round_trip",
	                                        test_include_data_titles_in_hri_round_trip },

	/* Symbology */
	{ "sym_default_is_none",                test_sym_default_is_none },
	{ "set_sym",                            test_set_sym },
	{ "set_sym_invalid_throws",             test_set_sym_invalid_throws },

	/* Validation */
	{ "validation_round_trip",              test_validation_round_trip },

	/* AI data */
	{ "set_ai_data_str_round_trip",         test_set_ai_data_str_round_trip },
	{ "set_ai_data_str_invalid_throws",     test_set_ai_data_str_invalid_throws },
	{ "set_data_str_dl_uri_round_trip",     test_set_data_str_dl_uri_round_trip },

	/* DL URI */
	{ "get_dl_uri",                         test_get_dl_uri },
	{ "get_dl_uri_canonical_stem",          test_get_dl_uri_canonical_stem },

	/* HRI */
	{ "hri_lines",                          test_hri_lines },
	{ "hri_empty_when_no_data",             test_hri_empty_when_no_data },

	/* Scan data */
	{ "set_scan_data",                      test_set_scan_data },
	{ "set_scan_data_invalid_throws",       test_set_scan_data_invalid_throws },

	{ NULL, NULL }
};
