/*
 * Wrapper class for accessing the functions exported by the GS1 Barcode Syntax
 * Engine native library (via the "bridging header") from Swift
 *
 * Copyright (c) 2022-2025 GS1 AISBL.
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
 * This class is a very lightweight shim around the native library,
 * therefore the Swift interface is described here in terms of the public
 * API functions of the native library that each method invokes.
 *
 * The API reference for the native C library is available here:
 *
 * https://gs1.github.io/gs1-syntax-engine/
 *
 */

import Foundation

/// A Swift wrapper around the GS1 Barcode Syntax Engine native library
class GS1Encoder {

    /**
     * List of symbology types, mirroring the corresponding list in the
     * C library.
     *
     * See the native library documentation for details:
     *
     *   - enum gs1_encoder_symbologies
     *
     */
    enum Symbology: Int32 {

        /// None defined
        case NONE = -1,

        /// GS1 DataBar Omnidirectional
        DataBarOmni,

        /// GS1 DataBar Truncated
        DataBarTruncated,

        /// GS1 DataBar Stacked
        DataBarStacked,

        /// GS1 DataBar Stacked Omnidirectional
        DataBarStackedOmni,

        /// GS1 DataBar Liimited
        DataBarLimited,

        /// GS1 DataBar Expanded (Stacked)
        DataBarExpanded,

        /// UPC-A
        UPCA,

        /// UPC-E
        UPCE,

        /// EAN-13
        EAN13,

        /// EAN-8
        EAN8,

        /// GS1-128 with CC-A or CC-B
        GS1_128_CCA,

        /// GS1-128 with CC-C
        GS1_128_CCC,

        /// (GS1) QR Code
        QR,

        /// (GS1) Data Matrix
        DM,

        /// (GS1) DotCode
        DotCode,

        /// Value is the number of symbologies
        NUMSYMS
    };

    /**
     * List of validations, mirroring the corresponding list in the
     * C library.
     *
     * See the native library documentation for details:
     *
     *   - enum gs1_encoder_validations
     *
     */
    enum Validation: UInt32 {

        /// Mutually exclusive AIs
        case MutexAIs = 0,

        /// Mandatory associations between AIs
        RequisiteAIs,

        /// Repeated AIs having same value
        RepeatedAIs,

        /// Unknown AIs not permitted as GS1 DL URI data attributes
        UnknownAInotDLattr,

        /// Value is the number of valudations
        NUMVALIDATIONS
    };

    /**
     * An opaque pointer used by the native code to represent an
     * "instance" of the library. It is hidden behind the object
     * interface that is provided to users of this wrapper.
     *
     * See the native library documentation for details:
     *
     *   - typedef struct gs1_encoder
     *
     */
    private(set) var ctx :OpaquePointer? = nil

    /*
     * This Swift wrapper library throws an exeception containing the error message whenever
     * an error is returned by the native library. Therefore direct access to the native
     * error message is not necessary.
     *
     */
    private func getErrMsg() -> String {
        return String(cString: gs1_encoder_getErrMsg(ctx))
    }

    /**
     * Constructor that creates an object wrapping an "instance" of the library
     * managed by the native code.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_init()
     *
     */
    init() throws {
        ctx = gs1_encoder_init(nil)
        if (ctx == nil) {
            throw GS1EncoderError.generalError(msg: "Failed to initalise the native library")
        }
    }

    /**
     * Destructor that will release the resources allocated by the native library.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_free()
     *
     */
    func free() {
        gs1_encoder_free(ctx)
        ctx = nil
    }

    /**
     * Returns the version of the native library.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getVersion()
     *
     */
    func getVersion() -> String {
        return String(cString: gs1_encoder_getVersion())
    }

    /**
     * Get the symbology type.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getVersion()
     *
     */
    func getSym() -> Symbology {
        return Symbology(rawValue: gs1_encoder_getSym(ctx).rawValue) ?? Symbology.NONE
    }

    /**
     * Set the symbology type.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setVersion()
     *
     */
    func setSym(_ value: Symbology) throws {
        if (!gs1_encoder_setSym(ctx, gs1_encoder_symbologies(value.rawValue))) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getAddCheckDigit()
     *
     */
    func getAddCheckDigit() -> Bool {
        return gs1_encoder_getAddCheckDigit(ctx)
    }

    /**
     * Set the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setAddCheckDigit()
     *
     */
    func setAddCheckDigit(_ value: Bool) throws {
        if (!gs1_encoder_setAddCheckDigit(ctx, value)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the "include data titles in HRI" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getIncludeDataTitlesInHRI()
     *
     */
    func getIncludeDataTitlesInHRI() -> Bool {
        return gs1_encoder_getIncludeDataTitlesInHRI(ctx)
    }

    /**
     * Set the "include data titles in HRI" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setIncludeDataTitlesInHRI()
     *
     */
    func setIncludeDataTitlesInHRI(_ value: Bool) throws {
        if (!gs1_encoder_setIncludeDataTitlesInHRI(ctx, value)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the "permit unknown AIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getPermitUnknownAIs()
     *
     */
    func getPermitUnknownAIs() -> Bool {
        return gs1_encoder_getPermitUnknownAIs(ctx)
    }

    /**
     * Set the "permit unknown AIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setPermitUnknownAIs()
     *
     */
    func setPermitUnknownAIs(_ value: Bool) throws {
        if (!gs1_encoder_setPermitUnknownAIs(ctx, value)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getPermitZeroSuppressedGTINinDLuris()
     *
     */
    func getPermitZeroSuppressedGTINinDLuris() -> Bool {
        return gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx)
    }

    /**
     * Set the status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setPermitZeroSuppressedGTINinDLuris()
     *
     */
    func setPermitZeroSuppressedGTINinDLuris(_ value: Bool) throws {
        if (!gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, value)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }
    
    /**
     * Get the checking of mandatory associations is enabled.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getValidationEnabled()
     *
     */
    func getValidationEnabled(_ validation: Validation) -> Bool {
        return gs1_encoder_getValidationEnabled(ctx, gs1_encoder_validations(validation.rawValue))
    }

    /**
     * Set the checking of mandatory associations.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setValidationEnabled()
     *
     */
    func setValidationEnabled(validation: Validation, enabled: Bool) throws {
        if (!gs1_encoder_setValidationEnabled(ctx, gs1_encoder_validations(validation.rawValue), enabled)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }
    
    /**
     * Get the "validate AI associations" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getValidateAIassociations()
     *
     */
    @available(*, deprecated)
    func getValidateAIassociations() -> Bool {
        return gs1_encoder_getValidateAIassociations(ctx)
    }

    /**
     * Set the "validate AI associations" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setValidateAIassociations()
     *
     */
    @available(*, deprecated)
    func setValidateAIassociations(_ value: Bool) throws {
        if (!gs1_encoder_setValidateAIassociations(ctx, value)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the raw barcode data input buffer.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getDataStr()
     *
     */
    func getDataStr() -> String {
        return String(cString: gs1_encoder_getDataStr(ctx))
    }

    /**
     * Set the raw barcode data input buffer.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setDataStr()
     *
     */
    func setDataStr(_ value: String) throws {
        if (!gs1_encoder_setDataStr(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String))) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the barcode data input buffer using GS1 AI syntax.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getAIdataStr()
     *
     */
    func getAIdataStr() -> String? {
        let cstr = gs1_encoder_getAIdataStr(ctx)
        if (cstr == nil) {
            return nil
        }
        return String(cString: cstr!)
    }

    /**
     * Set the barcode data input buffer using GS1 AI syntax.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setAIdataStr()
     *
     */
    func setAIdataStr(_ value: String) throws {
        if (!gs1_encoder_setAIdataStr(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String))) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /**
     * Get the barcode data input buffer using barcode scan data format.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getScanData()
     *
     */
    func getScanData() -> String {
        return String(cString: gs1_encoder_getScanData(ctx))
    }

    /**
     * Set the barcode data input buffer using barcode scan data format.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setScanData()
     *
     */
    func setScanData(_ value: String) throws {
        if (!gs1_encoder_setScanData(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String))) {
            throw GS1EncoderError.scanDataError(msg: self.getErrMsg())
        }
    }

    /**
     * Read the error markup generated when parsing AI data fails due to a
     * linting failure.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getErrMarkup()
     *
     */
    func getErrMarkup() -> String {
        return String(cString: gs1_encoder_getErrMarkup(ctx))
    }

    /**
     * Get a GS1 Digital Link URI that represents the AI-based input data.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getDLuri()
     *
     */
    func getDLuri(_ stem: String? = nil) throws -> String {
        var cstr: UnsafeMutablePointer<CChar>?;
        if let ustem = stem {
            cstr = gs1_encoder_getDLuri(ctx, UnsafeMutablePointer<CChar>(mutating: (ustem as NSString).utf8String))
        } else {
            cstr = gs1_encoder_getDLuri(ctx, nil)
        }
        if (cstr == nil) {
            throw GS1EncoderError.digitalLinkError(msg: self.getErrMsg())
        }
        return String(cString: cstr!)
    }

    /**
     * Get the Human-Readable Interpretation ("HRI") text for the current data
     * input buffer as an array of strings.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getHRI()
     *
     */
    func getHRI() -> Array<String> {
        let hri = UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<CChar>?>?>.allocate(capacity: 1)
        defer {
            hri.deinitialize(count: 1)
            hri.deallocate()
        }
        let numHRI = Int(gs1_encoder_getHRI(ctx, hri));
        var out = [String]()
        for (_, value) in UnsafeBufferPointer(start: hri.pointee, count: numHRI).enumerated() {
            out.append(String(cString: value!))
        }
        return out;
    }

    /**
     * Get the non-numeric (ignored) query parameters from a GS1 Digital Link
     * URI in the current data input buffer as an array of strings.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getDLignoredQueryParams()
     *
     */
    func getDLignoredQueryParams() -> Array<String> {
        let qp = UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<CChar>?>?>.allocate(capacity: 1)
        defer {
            qp.deinitialize(count: 1)
            qp.deallocate()
        }
        let numQP = Int(gs1_encoder_getDLignoredQueryParams(ctx, qp));
        var out = [String]()
        for (_, value) in UnsafeBufferPointer(start: qp.pointee, count: numQP).enumerated() {
            out.append(String(cString: value!))
        }
        return out;
    }

}

/*
 * Custom error classes for the wrapper for accessing the GS1 Barcode Syntax
 * Engine native library from Swift.
 */
enum GS1EncoderError: Error {
    case generalError(msg: String)
    case parameterError(msg: String)
    case scanDataError(msg: String)
    case digitalLinkError(msg: String)
}
