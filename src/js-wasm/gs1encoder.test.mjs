/**
 *  Basic tests for the JavaScript wrapper for the GS1 Barcode Syntax Engine compiled
 *  as a WASM by Emscripten.
 *
 *  Copyright (c) 2024-2026 GS1 AISBL.
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

import { GS1encoder, GS1encoderParameterException, GS1encoderDigitalLinkException } from "./gs1encoder.mjs";

const gs1encoder = new GS1encoder();

test('setDLuri', async () => {
  await gs1encoder.init();

  expect(() => { gs1encoder.dataStr = "https://id.example.org/test/01/12312312312319?99=TESTING123" }).not.toThrow();

  expect(gs1encoder.dataStr).toBe("https://id.example.org/test/01/12312312312319?99=TESTING123");
  expect(gs1encoder.getDLuri()).toBe("https://id.gs1.org/01/12312312312319?99=TESTING123");
  expect(gs1encoder.aiDataStr).toBe("(01)12312312312319(99)TESTING123");

  expect(() => { gs1encoder.includeDataTitlesInHRI = true }).not.toThrow();
  expect(gs1encoder.hri).toStrictEqual(["GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"]);

  expect(() => { gs1encoder.sym = GS1encoder.symbology.DM }).not.toThrow();
  expect(gs1encoder.sym).toBe(GS1encoder.symbology.DM);
  expect(gs1encoder.scanData).toBe("]d1https://id.example.org/test/01/12312312312319?99=TESTING123");

  gs1encoder.free();
});

test('setAIdataStr', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.aiDataStr = "(01)12312312312319(99)TESTING123" }).not.toThrow();

  expect(gs1encoder.dataStr).toBe("^011231231231231999TESTING123");
  expect(gs1encoder.getDLuri()).toBe("https://id.gs1.org/01/12312312312319?99=TESTING123");
  expect(gs1encoder.aiDataStr).toBe("(01)12312312312319(99)TESTING123");
  expect(gs1encoder.hri).toStrictEqual(["(01) 12312312312319", "(99) TESTING123"]);

  expect(() => { gs1encoder.sym = GS1encoder.symbology.QR }).not.toThrow();
  expect(gs1encoder.sym).toBe(GS1encoder.symbology.QR);
  expect(gs1encoder.scanData).toBe("]Q3011231231231231999TESTING123");

  gs1encoder.free();
});

test('requisites', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)).toBe(true);
  expect(() => { gs1encoder.dataStr = "^0212312312312319" }).toThrow(GS1encoderParameterException);
  expect(() => { gs1encoder.dataStr = "^0212312312312319" }).toThrow(/not satisfied/);

  expect(() => { gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, false) }).not.toThrow();
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)).toBe(false);
  expect(() => { gs1encoder.dataStr = "^0212312312312319" }).not.toThrow();

  expect(gs1encoder.dataStr).toBe("^0212312312312319");
  expect(() => { gs1encoder.getDLuri() }).toThrow(GS1encoderDigitalLinkException);
  expect(() => { gs1encoder.getDLuri() }).toThrow(/without a primary key/);
  expect(gs1encoder.aiDataStr).toBe("(02)12312312312319");
  expect(gs1encoder.hri).toStrictEqual(["(02) 12312312312319"]);

  expect(() => { gs1encoder.sym = GS1encoder.symbology.DataBarExpanded }).not.toThrow();
  expect(gs1encoder.sym).toBe(GS1encoder.symbology.DataBarExpanded);
  expect(gs1encoder.scanData).toBe("]e00212312312312319");

  gs1encoder.free();
});

test('create', async () => {
  const gs1encoder = await GS1encoder.create();

  expect(gs1encoder).toBeInstanceOf(GS1encoder);
  expect(gs1encoder.version).toBeTruthy();

  gs1encoder.free();
});

test('version', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(gs1encoder.version).toBeTruthy();
  expect(gs1encoder.version.length).toBeGreaterThan(0);

  gs1encoder.free();
});

test('defaults', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(gs1encoder.sym).toBe(GS1encoder.symbology.NONE);
  expect(gs1encoder.dataStr).toBe("");
  expect(gs1encoder.addCheckDigit).toBe(false);
  expect(gs1encoder.permitUnknownAIs).toBe(false);
  expect(gs1encoder.permitZeroSuppressedGTINinDLuris).toBe(false);
  expect(gs1encoder.includeDataTitlesInHRI).toBe(false);
  expect(gs1encoder.aiDataStr).toBeNull();
  expect(gs1encoder.scanData).toBeNull();
  expect(gs1encoder.hri).toStrictEqual([]);
  expect(gs1encoder.dlIgnoredQueryParams).toStrictEqual([]);
  expect(gs1encoder.errMarkup).toBe("");

  gs1encoder.free();
});

test('booleanSetters', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(gs1encoder.addCheckDigit).toBe(false);
  gs1encoder.addCheckDigit = true;
  expect(gs1encoder.addCheckDigit).toBe(true);
  gs1encoder.addCheckDigit = false;
  expect(gs1encoder.addCheckDigit).toBe(false);

  expect(gs1encoder.permitUnknownAIs).toBe(false);
  gs1encoder.permitUnknownAIs = true;
  expect(gs1encoder.permitUnknownAIs).toBe(true);
  gs1encoder.permitUnknownAIs = false;
  expect(gs1encoder.permitUnknownAIs).toBe(false);

  expect(gs1encoder.permitZeroSuppressedGTINinDLuris).toBe(false);
  gs1encoder.permitZeroSuppressedGTINinDLuris = true;
  expect(gs1encoder.permitZeroSuppressedGTINinDLuris).toBe(true);
  gs1encoder.permitZeroSuppressedGTINinDLuris = false;
  expect(gs1encoder.permitZeroSuppressedGTINinDLuris).toBe(false);

  expect(gs1encoder.includeDataTitlesInHRI).toBe(false);
  gs1encoder.includeDataTitlesInHRI = true;
  expect(gs1encoder.includeDataTitlesInHRI).toBe(true);
  gs1encoder.includeDataTitlesInHRI = false;
  expect(gs1encoder.includeDataTitlesInHRI).toBe(false);

  gs1encoder.free();
});

test('validations', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.MutexAIs)).toBe(true);
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)).toBe(true);
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RepeatedAIs)).toBe(true);
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.DigSigSerialKey)).toBe(true);
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.UnknownAInotDLattr)).toBe(true);

  expect(() => { gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, false) }).not.toThrow();
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)).toBe(false);
  expect(() => { gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, true) }).not.toThrow();
  expect(gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)).toBe(true);

  expect(() => { gs1encoder.setValidationEnabled(GS1encoder.validation.RepeatedAIs, false) }).toThrow(GS1encoderParameterException);
  expect(() => { gs1encoder.setValidationEnabled(GS1encoder.validation.RepeatedAIs, false) }).toThrow(/cannot be amended/);

  gs1encoder.free();
});

test('setScanData', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.scanData = "]e0011231231231233310ABC123\x1D99XYZ" }).not.toThrow();
  expect(gs1encoder.sym).toBe(GS1encoder.symbology.DataBarExpanded);
  expect(gs1encoder.dataStr).toBe("^011231231231233310ABC123^99XYZ");
  expect(gs1encoder.aiDataStr).toBe("(01)12312312312333(10)ABC123(99)XYZ");

  gs1encoder.free();
});

test('nonAIdata', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.dataStr = "TESTING" }).not.toThrow();
  expect(gs1encoder.aiDataStr).toBeNull();
  expect(gs1encoder.scanData).toBeNull();
  expect(gs1encoder.hri).toStrictEqual([]);
  expect(gs1encoder.dlIgnoredQueryParams).toStrictEqual([]);
  expect(() => { gs1encoder.getDLuri(null) }).toThrow(GS1encoderDigitalLinkException);
  expect(() => { gs1encoder.getDLuri(null) }).toThrow(/without a primary key/);

  gs1encoder.free();
});

test('getDLuriWithStem', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.aiDataStr = "(01)12312312312319" }).not.toThrow();
  expect(gs1encoder.getDLuri("https://example.com")).toMatch(/^https:\/\/example\.com\//);
  expect(gs1encoder.getDLuri(null)).toMatch(/^https:\/\/id\.gs1\.org\//);

  gs1encoder.free();
});

test('dlIgnoredQueryParams', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.dataStr = "https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ" }).not.toThrow();
  expect(gs1encoder.dlIgnoredQueryParams).toStrictEqual(["singleton", "compound=XYZ"]);
  expect(gs1encoder.hri).toStrictEqual(["(01) 12312312312333", "(22) TESTING", "(99) ABC"]);

  gs1encoder.free();
});

test('errMarkup', async () => {
  const gs1encoder = new GS1encoder();
  await gs1encoder.init();

  expect(() => { gs1encoder.dataStr = "^011234567890128399ABC" }).toThrow(GS1encoderParameterException);
  expect(() => { gs1encoder.dataStr = "^011234567890128399ABC" }).toThrow(/check digit/);
  expect(gs1encoder.errMarkup).not.toBe("");
  expect(gs1encoder.errMarkup).toMatch(/\|/);

  gs1encoder.free();
});
