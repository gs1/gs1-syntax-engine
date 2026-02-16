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

}
