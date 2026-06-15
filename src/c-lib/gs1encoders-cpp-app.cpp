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

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "gs1encoders.hpp"

namespace {

namespace enc = gs1encoders;

constexpr std::string_view RELEASE = __DATE__;


/* ------------------------------------------------------------------
 *  Small helpers
 * ------------------------------------------------------------------ */

[[nodiscard]] int parse_int_or(std::string_view s, int fallback) {
	int v;
	const auto [_, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
	return ec == std::errc{} ? v : fallback;
}

template <typename Table>
auto find_by_key(const Table &table, int key) {
	return std::find_if(table.begin(), table.end(),
	                    [key](const auto &o) { return o.key == key; });
}


/* ------------------------------------------------------------------
 *  Menu options as data
 * ------------------------------------------------------------------ */

// A menu entry that consumes a string input via one of the wrapper's
// data setters. Non-capturing lambdas decay to function pointers, so
// the whole table is a compile-time constant.
struct DataOption {
	int key;
	void (*set)(enc::GS1Encoder &, const std::string &);
};

constexpr std::array<DataOption, 3> data_options = {{
	{1, [](enc::GS1Encoder &g, const std::string &s) { g.set_data_str(s);    }},
	{2, [](enc::GS1Encoder &g, const std::string &s) { g.set_ai_data_str(s); }},
	{3, [](enc::GS1Encoder &g, const std::string &s) { g.set_scan_data(s);   }},
}};

// A boolean configuration flag exposed via a getter/setter pair on the
// wrapper. The lambdas capture nothing, so each entry is just a pair of
// function pointers carried in a constexpr table.
struct BoolOption {
	int              key;
	std::string_view label;
	bool (*get)(const enc::GS1Encoder &);
	void (*set)(enc::GS1Encoder &, bool);
};

constexpr std::array<BoolOption, 4> bool_options = {{
	{4, "include data titles in HRI",
	    [](const enc::GS1Encoder &g)         { return g.include_data_titles_in_hri();      },
	    [](enc::GS1Encoder       &g, bool b) {        g.set_include_data_titles_in_hri(b); }},
	{5, "permit unknown AIs",
	    [](const enc::GS1Encoder &g)         { return g.permit_unknown_ais();               },
	    [](enc::GS1Encoder       &g, bool b) {        g.set_permit_unknown_ais(b);          }},
	{6, "validate AI mandatory associations",
	    [](const enc::GS1Encoder &g)         { return g.validation_enabled(enc::Validation::RequisiteAIs);        },
	    [](enc::GS1Encoder       &g, bool b) {        g.set_validation_enabled(enc::Validation::RequisiteAIs, b); }},
	{7, "permit zero-suppressed GTIN in GS1 DL URIs",
	    [](const enc::GS1Encoder &g)         { return g.permit_zero_suppressed_gtin_in_dl_uris();      },
	    [](enc::GS1Encoder       &g, bool b) {        g.set_permit_zero_suppressed_gtin_in_dl_uris(b); }},
}};


/* ------------------------------------------------------------------
 *  Rendering
 * ------------------------------------------------------------------ */

void render_state(const enc::GS1Encoder &gs) {

	std::cout << "\n\n\nCurrent state:";

	const auto dataStr = gs.data_str();
	std::cout << "\n    Barcode message:        " << dataStr;

	const std::string aiDataStr = dataStr.empty() ? std::string{} : gs.ai_data_str();
	std::cout << "\n    AI element string:      "
	          << (aiDataStr.empty() ? "⧚ Not AI-based data ⧚" : aiDataStr.c_str());

	std::cout << "\n    GS1 Digital Link URI:   ";
	if (!dataStr.empty()) {
		try {
			std::cout << gs.get_dl_uri("");
		} catch (const enc::GS1EncoderException &e) {
			std::cout << "⧚ " << e.what() << " ⧚";
		}
	}

	const auto hri = dataStr.empty() ? std::vector<std::string>{} : gs.hri();
	std::cout << "\n    HRI:                    "
	          << (!dataStr.empty() && hri.empty() ? "⧚ Not AI-based data ⧚" : "")
	          << "\n";
	for (const auto &line : hri)
		std::cout << "        " << line << "\n";

}

void render_menu(const enc::GS1Encoder &gs) {

	std::cout << "\n\n\nMENU:";
	std::cout << "\n\n 1) Process raw barcode message data, either:";
	std::cout << "\n      * Plain data";
	std::cout << "\n      * Unbracketed AI element string with FNC1 in first position";
	std::cout << "\n      * GS1 Digital Link URI";
	std::cout << "\n 2) Process a bracketed AI element string";
	std::cout << "\n 3) Process barcode scan data (prefixed by AIM Symbology Identifier)";
	std::cout << "\n";
	for (const auto &opt : bool_options)
		std::cout << "\n " << opt.key << ") Set '" << opt.label
		          << "' flag.  Current value = " << (opt.get(gs) ? "ON" : "OFF");
	std::cout << "\n\n 0) Exit program";
	std::cout << "\n\nMenu selection: ";

}


/* ------------------------------------------------------------------
 *  Main loop
 * ------------------------------------------------------------------ */

void userInt(enc::GS1Encoder &gs) {

	while (true) {

		render_state(gs);
		render_menu(gs);

		std::string input;
		if (!std::getline(std::cin, input))
			return;

		const int menuVal = parse_int_or(input, -1);

		if (menuVal == 0)
			return;

		if (auto it = find_by_key(data_options, menuVal); it != data_options.end()) {
			std::cout << "\nEnter data: ";
			if (!std::getline(std::cin, input))
				continue;
			try {
				it->set(gs, input);
			} catch (const enc::GS1EncoderException &e) {
				std::cout << "\n\nERROR message: " << e.what() << "\n";
				if (const auto markup = gs.err_markup(); !markup.empty())
					std::cout << "ERROR markup:  " << markup << "\n";
			}
			continue;
		}

		if (auto it = find_by_key(bool_options, menuVal); it != bool_options.end()) {
			std::cout << "\nEnter 0 for OFF or 1 for ON: ";
			if (!std::getline(std::cin, input))
				return;
			const int v = parse_int_or(input, -1);
			if (v != 0 && v != 1) {
				std::cout << "OUT OF RANGE. PLEASE ENTER 0 or 1";
				continue;
			}
			try {
				it->set(gs, v == 1);
			} catch (const enc::GS1EncoderException &e) {
				std::cout << "\n\nERROR: " << e.what() << "\n";
			}
			continue;
		}

		std::cout << "OUT OF RANGE. PLEASE ENTER 1-7 or 0.";

	}

}

} // namespace


int main(int argc, char *argv[]) {

	const std::vector<std::string_view> args(argv + 1, argv + argc);
	std::string syntaxDictionary;
	bool showVersion = false;

	for (auto it = args.begin(); it != args.end(); ++it) {
		if (*it == "--syndict") {
			if (const auto next = std::next(it); next != args.end()) {
				syntaxDictionary = *next;
				it = next;
			}
		} else if (*it == "--version") {
			showVersion = true;
		}
	}

	/*
	 *  An explicit --syndict path is honoured strictly: init fails if
	 *  the file cannot be loaded. Without --syndict the console app
	 *  attempts to pick up gs1-syntax-dictionary.txt from local storage
	 *  (here, the current directory), falling back to the embedded AI
	 *  table if the file is absent or malformed. Loading the dictionary
	 *  from a file lets an application adopt a newer revision of the
	 *  Syntax Dictionary by replacing this file, without rebuilding the
	 *  library.
	 *
	 */
	enc::InitOpts opts;
	if (!syntaxDictionary.empty()) {
		opts.syntax_dictionary(syntaxDictionary);
	} else {
		opts.syntax_dictionary("gs1-syntax-dictionary.txt")
		    .fallback_on_syndict_error(true);
	}

	try {

		enc::GS1Encoder gs(opts);

		if (const auto warning = gs.init_fallback_warning())
			std::cout << "\n!!! Warning: " << *warning << "\n";

		if (showVersion) {
			std::cout << "Application version: " << RELEASE << "\n";
			std::cout << "Library version: " << gs.version() << "\n";
			return EXIT_SUCCESS;
		}

		std::cout << "\nGS1 Barcode Syntax Engine Console Demo (Built " << RELEASE << "):";
		std::cout << "\n\nCopyright (c) 2020-2026 GS1 AISBL. License: Apache-2.0";

		userInt(gs);

	} catch (const enc::GS1EncoderGeneralException &e) {
		std::cout << "Failed to initialise GS1 Encoders library!\n";
		std::cout << "\n!!! Error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}
