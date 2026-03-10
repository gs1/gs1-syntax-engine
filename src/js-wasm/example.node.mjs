/*
 *  Basic node.js example for using the JavaScript wrapper around the WASM or
 *  JS-only build of the GS1 Barcode Syntax Engine.
 *
 *  Requirements:
 *
 *    - WASM or JS-only build of the Syntax Engine in the current directory or
 *      the gs1encoder package installed via npm
 *
 *    - Any maintained version of Node.js
 *
 *  The API reference for the native C library is available here:
 *
 *      https://gs1.github.io/gs1-syntax-engine/
 *
 *
 *  Copyright (c) 2022-2026 GS1 AISBL.
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
 */

"use strict";


/*
 *  Setup for synchronous console interaction
 *
 */
import readline from "node:readline/promises";
import { stdin as input, stdout as output } from "node:process";
const rl = readline.createInterface({ input, output });

/*
 *  To run this example with an instance of gs1encoder that you have installed
 *  from npm change the following import to:
 *
 *    import { GS1encoder } from "gs1encoder";
 *
 */
import { GS1encoder } from "./gs1encoder.mjs";

const gs1encoder = await GS1encoder.create();

let exit = 0;


/*
 *  If --version is provided, then just report the library version
 *
 */
const argv = process.argv.slice(2);
if (argv.length == 1 && argv == "--version") {
    console.log("Library version: %s", gs1encoder.version);
    exit = 1;
}


/*
 *  Interactive loop
 *
 */
while (!exit) {

    let dataStr = gs1encoder.dataStr;
    let aiDataStr = "";
    let dlURI = "";
    let hri = [];
    if (dataStr !== "") {
        aiDataStr = gs1encoder.aiDataStr ?? "⧚ Not AI-based data ⧚";
        try { dlURI = gs1encoder.getDLuri(null); } catch (err) { dlURI = "⧚ " + err.message + " ⧚"; }
        hri = gs1encoder.hri;
    }

    console.log("\n\n\nCurrent state:");

    console.log("\n    Barcode message:        %s", dataStr);
    console.log("\n    AI element string:      %s", aiDataStr);
    console.log("\n    GS1 Digital Link URI:   %s", dlURI);
    console.log("\n    HRI:                    %s\n", dataStr !== "" && hri.length == 0 ? "⧚ Not AI-based data ⧚": "");
    hri.forEach(ai => console.log("       %s", ai));

    console.log("\n\nMENU:");
    console.log("\n 1) Process raw barcode message data, either:");
    console.log("      * Plain data");
    console.log("      * Unbracketed AI element string with FNC1 in first position");
    console.log("      * GS1 Digital Link URI");
    console.log(" 2) Process a bracketed AI element string");
    console.log(" 3) Process barcode scan data (prefixed by AIM Symbology Identifier)");

    console.log("\n 4) Set 'include data titles in HRI' flag.                  Current value = %s",
                                        gs1encoder.includeDataTitlesInHRI ? "ON" : "OFF");
    console.log(" 5) Set 'permit unknown AIs' flag.                          Current value = %s",
                                        gs1encoder.permitUnknownAIs ? "ON" : "OFF");
    console.log(" 6) Set 'validate AI requisites'.                           Current value = %s",
                                        gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs) ? "ON" : "OFF");
    console.log(" 7) Set 'permit zero-suppressed GTIN in GS1 DL URIs' flag.  Current value = %s",
                                        gs1encoder.permitZeroSuppressedGTINinDLuris ? "ON" : "OFF");

    console.log("\n 0) Exit program");

    const menuVal = await rl.question("\nMenu selection: ");

    switch (menuVal) {
        case "1":
        case "2":
        case "3": {
            const inpStr = await rl.question("\nEnter data: ");
            if (!inpStr)
                continue;
            try {
                if (menuVal === "1")
                    gs1encoder.dataStr = inpStr;
                else if (menuVal === "2")
                    gs1encoder.aiDataStr = inpStr;
                else            // "3"
                    gs1encoder.scanData = inpStr;
            } catch (err) {
                console.log("\n\nERROR message: %s", err.message);
                const markup = gs1encoder.errMarkup;
                if (markup)
                    console.log("ERROR markup:  %s", markup.replace(/\|/g, "⧚"));
                continue;
            }
            break;
        }
        case "4":
        case "5":
        case "6":
        case "7": {
            const inpStr = await rl.question("\nEnter 0 for OFF or 1 for ON: ");
            if (!inpStr)
                continue;
            if (inpStr !== "0" && inpStr !== "1") {
                console.log("\n\nOUT OF RANGE. PLEASE ENTER 0 or 1");
                continue;
            }
            try {
                if (menuVal === "4")
                    gs1encoder.includeDataTitlesInHRI = (inpStr === "1");
                else if (menuVal === "5")
                    gs1encoder.permitUnknownAIs = (inpStr === "1");
                else if (menuVal === "6")
                    gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, inpStr === "1");
                else if (menuVal === "7")
                    gs1encoder.permitZeroSuppressedGTINinDLuris = (inpStr === "1");
            } catch (err) {
                console.log("\n\nERROR: %s", err.message);
                continue;
            }
            break;
        }
        case "0":
            exit = 1;
            break;
        default:
            console.log("\n\nOUT OF RANGE. PLEASE ENTER 1-6, 9 or 0.");
    }

}

gs1encoder.free();

rl.close();
