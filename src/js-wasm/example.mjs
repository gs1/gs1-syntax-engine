/*
 *  Event-driven functions for example HTML app. which uses the JavaScript
 *  wrapper around the WASM build of the GS1 Syntax Engine.
 *
 *  Copyright (c) 2022-2023 GS1 AISBL.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  The API reference for the native C library is available here:
 *
 *  https://gs1.github.io/gs1-syntax-engine/
 *
 */

'use strict';

import { GS1encoder } from './gs1encoder.mjs';


var gs1encoder = {};


async function init() {

	/*
	 * Use the wrapper to initialise the WASM library
	 *
	 */
	gs1encoder = new GS1encoder();
	await gs1encoder.init();

	document.title = "GS1 Syntax Engine HTML demo | Library release: " + gs1encoder.version;

	document.getElementById("data").value = "https://example.com/01/12312312312333/10/ABC123?99=TESTING";

	load_data_values();

}


function clear_render() {
	document.getElementById("syntax").value = "";
	document.getElementById("datastr").value = "";
	document.getElementById("elementstring").value = "";
	document.getElementById("dl").value = "";
	document.getElementById("hri").value = "";
	document.getElementById("info").value = "";
	document.getElementById("errmsg").value = "";
}


function load_data_values() {

	document.getElementById("unknownais").checked = gs1encoder.permitUnknownAIs;
	document.getElementById("associations").checked = gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs);
	document.getElementById("datatitles").checked = gs1encoder.includeDataTitlesInHRI;
	document.getElementById("zerosuppdl").checked = gs1encoder.permitZeroSuppressedGTINinDLuris;

	if (gs1encoder.dataStr == "") return;

	document.getElementById("datastr").value = gs1encoder.dataStr;
	document.getElementById("elementstring").value = gs1encoder.aiDataStr || "⧚ Not AI-based data ⧛";

	try {
		document.getElementById("dl").value = gs1encoder.getDLuri(null);
	}
	catch (err) {
		document.getElementById("dl").value = "⧚ " + err.message + " ⧛";
	}

	var hri = gs1encoder.hri;
	if (hri.length > 0) {
		for (const ai of hri)
			document.getElementById("hri").value += ai + "\n";
	} else {
		document.getElementById("hri").value = "⧚ Not AI-based data ⧛";
	}

	var qps = gs1encoder.dlIgnoredQueryParams;
	if (qps.length > 0)
		document.getElementById("info").value = "Warning: Non-numeric query parameters ignored: ⧚" +
			qps.join('&') + "⧚";

}


function process_clicked() {

	clear_render();

	try {

		var data = document.getElementById("data").value;
		if (data == "") return;

		if (data.startsWith('(')) {
			document.getElementById("syntax").value = "Bracketed AI element string";
			gs1encoder.aiDataStr = data;
		} else if (data.startsWith(']')) {
			document.getElementById("syntax").value = "Barcode scan data with AIM symbology identifier";
			gs1encoder.scanData = data.replace("{GS}", "\u001d");
		} else if (data.startsWith('^')) {
			document.getElementById("syntax").value = "Unbracketed AI element string";
			gs1encoder.dataStr = data;
		} else if (data.startsWith("http://") || data.startsWith("HTTP://") || data.startsWith("https://") || data.startsWith("HTTPS://")) {
			document.getElementById("syntax").value = "GS1 Digital Link URI";
			gs1encoder.dataStr = data;
		} else if (/^\d+$/.test(data)) {
			document.getElementById("syntax").value = "Plain data";

			if (data.length != 8 && data.length != 12 && data.length != 13 && data.length != 14) {
				document.getElementById("errmsg").value = "Invalid length for a GTIN-8, GTIN-12, GTIN-13 or GTIN-14";
				return;
			}

			// Perform a checksum validation here, since the Syntax Engine validates only AI-based data
			var parity = 0;
			var weight = data.length % 2 == 0 ? 3 : 1;
			var i;
			for (i = 0; i < data.length - 1; i++) {
				parity += weight * (data[i] - '0');
				weight = 4 - weight;
			}
			parity = (10 - parity % 10) % 10;

			if (parity != data[i]) {
				document.getElementById("errmsg").value = "Incorrect numeric check digit";
				document.getElementById("info").value = "Plain data validation failed: " + data.slice(0, -1) + "⧚" + data[i] + "⧛";
				return;
			}

			document.getElementById("syntax").value = "Plain GTIN-" + data.length + " - converted to AI (01)...";
			gs1encoder.dataStr = "^01" + data.padStart(14, '0');

		} else {
			document.getElementById("syntax").value = "Non-numeric plain data is not a valid GS1 syntax";
			return;
		}

	}
	catch (err) {
		document.getElementById("errmsg").value = "Error: " + err.message;
		var markup = gs1encoder.errMarkup;
		if (markup) {
			document.getElementById("info").value = "AI content validation failed: " + markup.replace(/\|/g, "⧚");
		}
		return;
	}

	load_data_values();

}


function data_changed() {
	clear_render();
}


function unknownais_clicked() {
	clear_render();
	gs1encoder.permitUnknownAIs = document.getElementById("unknownais").checked;
}


function associations_clicked() {
	clear_render();
	gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, document.getElementById("associations").checked);
}


function datatitles_clicked() {
	clear_render();
	gs1encoder.includeDataTitlesInHRI = document.getElementById("datatitles").checked;
}


function zerosuppdl_clicked() {
	clear_render();
	gs1encoder.permitZeroSuppressedGTINinDLuris = document.getElementById("zerosuppdl").checked;
}


window.init = init;
window.process_clicked = process_clicked;
window.data_changed = data_changed;
window.unknownais_clicked = unknownais_clicked;
window.associations_clicked = associations_clicked;
window.datatitles_clicked = datatitles_clicked;
window.zerosuppdl_clicked = zerosuppdl_clicked
