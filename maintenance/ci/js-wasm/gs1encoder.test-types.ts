/**
 * TypeScript type checking test for gs1encoder
 *
 * This file validates that the TypeScript definitions in gs1encoder.d.ts
 * are correct and complete. It is compiled (not executed) during CI to
 * ensure type definitions stay in sync with the implementation.
 *
 * Run: npx tsc --noEmit gs1encoder.test-types.ts
 */

import {
    GS1encoder,
    Symbology,
    Validation,
    GS1encoderGeneralException,
    GS1encoderParameterException,
    GS1encoderDigitalLinkException,
    GS1encoderScanDataException
} from '../../../src/js-wasm/gs1encoder';

async function testTypes(): Promise<void> {
    // Test constructor
    const encoder = new GS1encoder();

    // Test init
    await encoder.init();

    // Test version getter (string)
    const version: string = encoder.version;

    // Test errMarkup getter (string)
    const errMarkup: string = encoder.errMarkup;

    // Test sym getter/setter (Symbology enum)
    const sym: Symbology = encoder.sym;
    encoder.sym = Symbology.QR;
    encoder.sym = Symbology.DataBarOmni;
    encoder.sym = Symbology.NONE;

    // Test addCheckDigit getter/setter (boolean)
    const addCheckDigit: boolean = encoder.addCheckDigit;
    encoder.addCheckDigit = true;
    encoder.addCheckDigit = false;

    // Test permitUnknownAIs getter/setter (boolean)
    const permitUnknownAIs: boolean = encoder.permitUnknownAIs;
    encoder.permitUnknownAIs = true;

    // Test permitZeroSuppressedGTINinDLuris getter/setter (boolean)
    const permitZeroSuppressed: boolean = encoder.permitZeroSuppressedGTINinDLuris;
    encoder.permitZeroSuppressedGTINinDLuris = false;

    // Test includeDataTitlesInHRI getter/setter (boolean)
    const includeDataTitles: boolean = encoder.includeDataTitlesInHRI;
    encoder.includeDataTitlesInHRI = true;

    // Test validation methods
    const validationEnabled: boolean = encoder.getValidationEnabled(Validation.RequisiteAIs);
    encoder.setValidationEnabled(Validation.RequisiteAIs, false);
    encoder.setValidationEnabled(Validation.UnknownAInotDLattr, true);

    // Test deprecated validateAIassociations getter/setter (boolean)
    const validateAI: boolean = encoder.validateAIassociations;
    encoder.validateAIassociations = false;

    // Test aiDataStr getter/setter (string | null for getter, string for setter)
    const aiDataStr: string | null = encoder.aiDataStr;
    encoder.aiDataStr = "(01)12345678901231(10)ABC123";

    // Test dataStr getter/setter (string)
    const dataStr: string = encoder.dataStr;
    encoder.dataStr = "^0112345678901231";
    encoder.dataStr = "https://id.gs1.org/01/12345678901231";

    // Test getDLuri (returns string)
    const dlUri: string = encoder.getDLuri(null);
    const dlUriWithStem: string = encoder.getDLuri("https://example.com");

    // Test scanData getter/setter (string)
    const scanData: string = encoder.scanData;
    encoder.scanData = "]C1011231231231233310ABC123";

    // Test hri getter (string array)
    const hri: string[] = encoder.hri;
    const firstLine: string = hri[0];

    // Test dlIgnoredQueryParams getter (string array)
    const ignoredParams: string[] = encoder.dlIgnoredQueryParams;
    const firstParam: string = ignoredParams[0];

    // Test free
    encoder.free();

    // Test static symbology enum access
    const symbologyEnum = GS1encoder.symbology;
    const qrCode: Symbology = symbologyEnum.QR;
    const dm: Symbology = symbologyEnum.DM;

    // Test static validation enum access
    const validationEnum = GS1encoder.validation;
    const requisite: Validation = validationEnum.RequisiteAIs;

    // Test exception types
    try {
        encoder.aiDataStr = "invalid";
    } catch (e) {
        if (e instanceof GS1encoderParameterException) {
            // Exception type verified
        }
    }

    try {
        await new GS1encoder().init();
    } catch (e) {
        if (e instanceof GS1encoderGeneralException) {
            // Exception type verified
        }
    }

    try {
        encoder.getDLuri(null);
    } catch (e) {
        if (e instanceof GS1encoderDigitalLinkException) {
            // Exception type verified
        }
    }

    // Test all Symbology enum values
    const allSymbologies: Symbology[] = [
        Symbology.NONE,
        Symbology.DataBarOmni,
        Symbology.DataBarTruncated,
        Symbology.DataBarStacked,
        Symbology.DataBarStackedOmni,
        Symbology.DataBarLimited,
        Symbology.DataBarExpanded,
        Symbology.UPCA,
        Symbology.UPCE,
        Symbology.EAN13,
        Symbology.EAN8,
        Symbology.GS1_128_CCA,
        Symbology.GS1_128_CCC,
        Symbology.QR,
        Symbology.DM,
        Symbology.DotCode,
        Symbology.NUMSYMS,
    ];

    // Test all Validation enum values
    const allValidations: Validation[] = [
        Validation.MutexAIs,
        Validation.RequisiteAIs,
        Validation.RepeatedAIs,
        Validation.DigSigSerialKey,
        Validation.UnknownAInotDLattr,
        Validation.NUMVALIDATIONS,
    ];
}

// This function is never executed, only type-checked
export { testTypes };
