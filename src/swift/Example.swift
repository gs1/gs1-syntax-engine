/**
 * GS1 Barcode Syntax Engine example user of the Swift binding
 *
 * @author Copyright (c) 2022-2025 GS1 AISBL.
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

import Foundation
import GS1Encoders

func readLine() -> String {
    guard let line = Swift.readLine() else {
        return ""
    }
    return line
}

guard let gs1encoder = try? GS1Encoder() else {
    print("Failed to initialise the native library")
    Foundation.exit(1)
}

defer { gs1encoder.free() }

var shouldExit = false

if CommandLine.arguments.count == 2 && CommandLine.arguments[1] == "--version" {
    print("Library version: \(gs1encoder.getVersion())")
    shouldExit = true
}

while !shouldExit {

    let dataStr = gs1encoder.getDataStr()
    var aiDataStr = ""
    var dlURI = ""
    var hri: [String] = []

    if !dataStr.isEmpty {
        if let ai = gs1encoder.getAIdataStr() {
            aiDataStr = ai
        } else {
            aiDataStr = "⧚ Not AI-based data ⧚"
        }

        do {
            dlURI = try gs1encoder.getDLuri(nil)
        } catch {
            dlURI = "⧚ \(error) ⧚"
        }

        hri = gs1encoder.getHRI()
    }

    print("\n\n\nCurrent state:")

    print("\n    Barcode message:        \(dataStr)")
    print("    AI element string:      \(aiDataStr)")
    print("    GS1 Digital Link URI:   \(dlURI)")
    print("    HRI:                    \(!dataStr.isEmpty && hri.isEmpty ? "⧚ Not AI-based data ⧚" : "")")
    for h in hri {
        print("       \(h)")
    }

    print("\n\nMENU:")
    print("\n 1) Process raw barcode message data, either:")
    print("      * Plain data")
    print("      * Unbracketed AI element string with FNC1 in first position")
    print("      * GS1 Digital Link URI")
    print(" 2) Process a bracketed AI element string")
    print(" 3) Process barcode scan data (prefixed by AIM Symbology Identifier)")

    print("\n 4) Toggle 'include data titles in HRI' flag.  Current value = \(gs1encoder.getIncludeDataTitlesInHRI() ? "ON" : "OFF")")
    print(" 5) Toggle 'permit unknown AIs' flag.          Current value = \(gs1encoder.getPermitUnknownAIs() ? "ON" : "OFF")")
    print(" 6) Toggle 'validate AI requisites'.           Current value = \(gs1encoder.getValidationEnabled(.RequisiteAIs) ? "ON" : "OFF")")

    print("\n 0) Exit program")

    print("\nMenu selection: ", terminator: "")
    let menuVal = readLine()

    var inpStr: String

    switch menuVal {
    case "1", "2", "3":
        print("\nEnter data: ", terminator: "")
        inpStr = readLine()
        if inpStr.isEmpty {
            continue
        }

        do {
            if menuVal == "1" {
                try gs1encoder.setDataStr(inpStr)
            } else if menuVal == "2" {
                try gs1encoder.setAIdataStr(inpStr)
            } else {  // "3"
                try gs1encoder.setScanData(inpStr)
            }
        } catch {
            print("\n\nERROR message: \(error)\n")
            let markup = gs1encoder.getErrMarkup()
            if !markup.isEmpty {
                print("ERROR markup:  \(markup.replacingOccurrences(of: "|", with: "⧚"))")
            }
            continue
        }

    case "4", "5", "6":
        print("\nEnter 0 for OFF or 1 for ON: ", terminator: "")
        inpStr = readLine()
        if inpStr.isEmpty {
            continue
        }

        if inpStr != "0" && inpStr != "1" {
            print("\n\nOUT OF RANGE. PLEASE ENTER 0 or 1")
            continue
        }

        do {
            if menuVal == "4" {
                try gs1encoder.setIncludeDataTitlesInHRI(inpStr == "1")
            } else if menuVal == "5" {
                try gs1encoder.setPermitUnknownAIs(inpStr == "1")
            } else {  // "6"
                try gs1encoder.setValidationEnabled(validation: .RequisiteAIs, enabled: inpStr == "1")
            }
        } catch {
            print("\n\nERROR: \(error)")
            continue
        }

    case "0":
        shouldExit = true

    default:
        print("\n\nOUT OF RANGE. PLEASE ENTER 1-6 or 0.")
    }

}
