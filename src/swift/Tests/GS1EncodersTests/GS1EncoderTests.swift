/*
 * Basic tests for the Swift wrapper for the GS1 Barcode Syntax Engine.
 *
 * Copyright (c) 2026 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import XCTest
@testable import GS1Encoders

final class GS1EncoderTests: XCTestCase {

    func testSetDLuri() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setDataStr("https://id.example.org/test/01/12312312312319?99=TESTING123")

        XCTAssertEqual(gs1encoder.getDataStr(), "https://id.example.org/test/01/12312312312319?99=TESTING123")
        XCTAssertEqual(try gs1encoder.getDLuri(), "https://id.gs1.org/01/12312312312319?99=TESTING123")
        XCTAssertEqual(gs1encoder.getAIdataStr(), "(01)12312312312319(99)TESTING123")

        try gs1encoder.setIncludeDataTitlesInHRI(true)
        XCTAssertEqual(gs1encoder.getHRI(), ["GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"])

        try gs1encoder.setSym(.DM)
        XCTAssertEqual(gs1encoder.getSym(), .DM)
        XCTAssertEqual(gs1encoder.getScanData(), "]d1https://id.example.org/test/01/12312312312319?99=TESTING123")

        gs1encoder.free()
    }

    func testSetAIdataStr() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setAIdataStr("(01)12312312312319(99)TESTING123")

        XCTAssertEqual(gs1encoder.getDataStr(), "^011231231231231999TESTING123")
        XCTAssertEqual(try gs1encoder.getDLuri(), "https://id.gs1.org/01/12312312312319?99=TESTING123")
        XCTAssertEqual(gs1encoder.getAIdataStr(), "(01)12312312312319(99)TESTING123")
        XCTAssertEqual(gs1encoder.getHRI(), ["(01) 12312312312319", "(99) TESTING123"])

        try gs1encoder.setSym(.QR)
        XCTAssertEqual(gs1encoder.getSym(), .QR)
        XCTAssertEqual(gs1encoder.getScanData(), "]Q3011231231231231999TESTING123")

        gs1encoder.free()
    }

    func testRequisites() throws {
        let gs1encoder = try GS1Encoder()

        XCTAssertTrue(gs1encoder.getValidationEnabled(.RequisiteAIs))
        XCTAssertThrowsError(try gs1encoder.setDataStr("^0212312312312319")) { error in
            guard case GS1EncoderError.parameterError(let msg) = error else {
                XCTFail("Expected parameterError")
                return
            }
            XCTAssertTrue(msg.contains("not satisfied"), "Error message should contain 'not satisfied'")
        }

        try gs1encoder.setValidationEnabled(validation: .RequisiteAIs, enabled: false)
        XCTAssertFalse(gs1encoder.getValidationEnabled(.RequisiteAIs))
        try gs1encoder.setDataStr("^0212312312312319")

        XCTAssertEqual(gs1encoder.getDataStr(), "^0212312312312319")
        XCTAssertThrowsError(try gs1encoder.getDLuri()) { error in
            guard case GS1EncoderError.digitalLinkError(let msg) = error else {
                XCTFail("Expected digitalLinkError")
                return
            }
            XCTAssertTrue(msg.contains("without a primary key"), "Error message should contain 'without a primary key'")
        }
        XCTAssertEqual(gs1encoder.getAIdataStr(), "(02)12312312312319")
        XCTAssertEqual(gs1encoder.getHRI(), ["(02) 12312312312319"])

        try gs1encoder.setSym(.DataBarExpanded)
        XCTAssertEqual(gs1encoder.getSym(), .DataBarExpanded)
        XCTAssertEqual(gs1encoder.getScanData(), "]e00212312312312319")

        gs1encoder.free()
    }

    func testVersion() throws {
        let gs1encoder = try GS1Encoder()

        let version = gs1encoder.getVersion()
        XCTAssertFalse(version.isEmpty)

        gs1encoder.free()
    }

    func testDefaults() throws {
        let gs1encoder = try GS1Encoder()

        XCTAssertEqual(gs1encoder.getSym(), .NONE)
        XCTAssertEqual(gs1encoder.getDataStr(), "")
        XCTAssertFalse(gs1encoder.getAddCheckDigit())
        XCTAssertFalse(gs1encoder.getPermitUnknownAIs())
        XCTAssertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris())
        XCTAssertFalse(gs1encoder.getIncludeDataTitlesInHRI())
        XCTAssertNil(gs1encoder.getAIdataStr())
        XCTAssertNil(gs1encoder.getScanData())
        XCTAssertEqual(gs1encoder.getHRI(), [])
        XCTAssertEqual(gs1encoder.getDLignoredQueryParams(), [])
        XCTAssertEqual(gs1encoder.getErrMarkup(), "")

        gs1encoder.free()
    }

    func testBooleanSetters() throws {
        let gs1encoder = try GS1Encoder()

        XCTAssertFalse(gs1encoder.getAddCheckDigit())
        try gs1encoder.setAddCheckDigit(true)
        XCTAssertTrue(gs1encoder.getAddCheckDigit())
        try gs1encoder.setAddCheckDigit(false)
        XCTAssertFalse(gs1encoder.getAddCheckDigit())

        XCTAssertFalse(gs1encoder.getPermitUnknownAIs())
        try gs1encoder.setPermitUnknownAIs(true)
        XCTAssertTrue(gs1encoder.getPermitUnknownAIs())
        try gs1encoder.setPermitUnknownAIs(false)
        XCTAssertFalse(gs1encoder.getPermitUnknownAIs())

        XCTAssertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris())
        try gs1encoder.setPermitZeroSuppressedGTINinDLuris(true)
        XCTAssertTrue(gs1encoder.getPermitZeroSuppressedGTINinDLuris())
        try gs1encoder.setPermitZeroSuppressedGTINinDLuris(false)
        XCTAssertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris())

        XCTAssertFalse(gs1encoder.getIncludeDataTitlesInHRI())
        try gs1encoder.setIncludeDataTitlesInHRI(true)
        XCTAssertTrue(gs1encoder.getIncludeDataTitlesInHRI())
        try gs1encoder.setIncludeDataTitlesInHRI(false)
        XCTAssertFalse(gs1encoder.getIncludeDataTitlesInHRI())

        gs1encoder.free()
    }

    func testValidations() throws {
        let gs1encoder = try GS1Encoder()

        XCTAssertTrue(gs1encoder.getValidationEnabled(.MutexAIs))
        XCTAssertTrue(gs1encoder.getValidationEnabled(.RequisiteAIs))
        XCTAssertTrue(gs1encoder.getValidationEnabled(.RepeatedAIs))
        XCTAssertTrue(gs1encoder.getValidationEnabled(.DigSigSerialKey))
        XCTAssertTrue(gs1encoder.getValidationEnabled(.UnknownAInotDLattr))

        try gs1encoder.setValidationEnabled(validation: .RequisiteAIs, enabled: false)
        XCTAssertFalse(gs1encoder.getValidationEnabled(.RequisiteAIs))
        try gs1encoder.setValidationEnabled(validation: .RequisiteAIs, enabled: true)
        XCTAssertTrue(gs1encoder.getValidationEnabled(.RequisiteAIs))

        XCTAssertThrowsError(try gs1encoder.setValidationEnabled(validation: .RepeatedAIs, enabled: false)) { error in
            guard case GS1EncoderError.parameterError(let msg) = error else {
                XCTFail("Expected parameterError")
                return
            }
            XCTAssertTrue(msg.contains("cannot be amended"), "Error message should contain 'cannot be amended'")
        }

        gs1encoder.free()
    }

    func testSetScanData() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setScanData("]e0011231231231233310ABC123\u{1D}99XYZ")
        XCTAssertEqual(gs1encoder.getSym(), .DataBarExpanded)
        XCTAssertEqual(gs1encoder.getDataStr(), "^011231231231233310ABC123^99XYZ")
        XCTAssertEqual(gs1encoder.getAIdataStr(), "(01)12312312312333(10)ABC123(99)XYZ")

        gs1encoder.free()
    }

    func testNonAIdata() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setDataStr("TESTING")
        XCTAssertNil(gs1encoder.getAIdataStr())
        XCTAssertNil(gs1encoder.getScanData())
        XCTAssertEqual(gs1encoder.getHRI(), [])
        XCTAssertEqual(gs1encoder.getDLignoredQueryParams(), [])
        XCTAssertThrowsError(try gs1encoder.getDLuri()) { error in
            guard case GS1EncoderError.digitalLinkError(let msg) = error else {
                XCTFail("Expected digitalLinkError")
                return
            }
            XCTAssertTrue(msg.contains("without a primary key"), "Error message should contain 'without a primary key'")
        }

        gs1encoder.free()
    }

    func testGetDLuriWithStem() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setAIdataStr("(01)12312312312319")
        let customUri = try gs1encoder.getDLuri("https://example.com")
        XCTAssertTrue(customUri.hasPrefix("https://example.com/"))
        let defaultUri = try gs1encoder.getDLuri()
        XCTAssertTrue(defaultUri.hasPrefix("https://id.gs1.org/"))

        gs1encoder.free()
    }

    func testDLignoredQueryParams() throws {
        let gs1encoder = try GS1Encoder()

        try gs1encoder.setDataStr("https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ")
        XCTAssertEqual(gs1encoder.getDLignoredQueryParams(), ["singleton", "compound=XYZ"])
        XCTAssertEqual(gs1encoder.getHRI(), ["(01) 12312312312333", "(22) TESTING", "(99) ABC"])

        gs1encoder.free()
    }

    func testDeinit() throws {
        var gs1encoder: GS1Encoder? = try GS1Encoder()
        XCTAssertNotNil(gs1encoder!.getVersion())
        gs1encoder = nil  // Should trigger deinit and free native resources
    }

    func testErrMarkup() throws {
        let gs1encoder = try GS1Encoder()

        XCTAssertThrowsError(try gs1encoder.setDataStr("^011234567890128399ABC")) { error in
            guard case GS1EncoderError.parameterError(let msg) = error else {
                XCTFail("Expected parameterError")
                return
            }
            XCTAssertTrue(msg.contains("check digit"), "Error message should contain 'check digit'")
        }
        XCTAssertFalse(gs1encoder.getErrMarkup().isEmpty)
        XCTAssertTrue(gs1encoder.getErrMarkup().contains("|"), "Error markup should contain '|' delimiters")

        gs1encoder.free()
    }

}
