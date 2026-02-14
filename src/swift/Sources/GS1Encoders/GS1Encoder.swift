/*
 * Wrapper class for accessing the functions exported by the GS1 Barcode Syntax
 * Engine native library from Swift
 *
 * Copyright (c) 2022-2026 GS1 AISBL.
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

import Foundation
import CGS1Encoders

/// Main class for processing GS1 barcode data, including validation, format conversion, and generation of outputs such as GS1 Digital Link URIs and Human-Readable Interpretation text.
public class GS1Encoder {

    /// Recognised GS1 barcode formats ("symbologies") for processing scan data.
    ///
    /// This enum defines all supported GS1 barcode symbology types that can be used
    /// with the encoder. Each symbology has specific characteristics and use cases.
    public enum Symbology: Int32 {

        /// None defined
        case NONE = -1

        /// GS1 DataBar Omnidirectional
        case DataBarOmni

        /// GS1 DataBar Truncated
        case DataBarTruncated

        /// GS1 DataBar Stacked
        case DataBarStacked

        /// GS1 DataBar Stacked Omnidirectional
        case DataBarStackedOmni

        /// GS1 DataBar Limited
        case DataBarLimited

        /// GS1 DataBar Expanded (Stacked)
        case DataBarExpanded

        /// UPC-A
        case UPCA

        /// UPC-E
        case UPCE

        /// EAN-13
        case EAN13

        /// EAN-8
        case EAN8

        /// GS1-128 with CC-A or CC-B
        case GS1_128_CCA

        /// GS1-128 with CC-C
        case GS1_128_CCC

        /// (GS1) QR Code
        case QR

        /// (GS1) Data Matrix
        case DM

        /// (GS1) DotCode
        case DotCode

        /// Value is the number of symbologies
        case NUMSYMS
    }


    /// Optional AI validation procedures that may be applied to detect invalid inputs.
    ///
    /// These validation procedures are applied when AI data is provided using
    /// `setAIdataStr(_:)`, `setDataStr(_:)` or `setScanData(_:)`.
    ///
    /// - Note: Some validation procedures are "locked" (always enabled and cannot be modified).
    /// All validation procedures are listed to maintain correct enum value alignment with the native library.
    public enum Validation: UInt32 {

        /// Mutually exclusive AIs (locked: always enabled)
        case MutexAIs = 0

        /// Mandatory associations between AIs
        case RequisiteAIs

        /// Repeated AIs having same value (locked: always enabled)
        case RepeatedAIs

        /// Unknown AIs not permitted as GS1 DL URI data attributes
        case UnknownAInotDLattr

        /// Value is the number of validations
        case NUMVALIDATIONS
    }

    /// An opaque pointer used by the native code to represent an
    /// "instance" of the library. It is hidden behind the object
    /// interface that is provided to users of this wrapper.
    private(set) var ctx: OpaquePointer? = nil

    // This Swift wrapper library throws an exception containing the error message whenever
    // an error is returned by the native library. Therefore direct access to the native
    // error message is not necessary.
    private func getErrMsg() -> String {
        return String(cString: gs1_encoder_getErrMsg(ctx))
    }


    /// Initialises a new instance of the GS1Encoder class.
    ///
    /// - Throws: `GS1EncoderError.generalError` if the library fails to initialise
    public init() throws {
        ctx = gs1_encoder_init(nil)
        if ctx == nil {
            throw GS1EncoderError.generalError(msg: "Failed to initialise the native library")
        }
    }

    /// Release library resources.
    ///
    /// This method will release the resources allocated by the native library.
    /// After calling this method, the GS1Encoder instance should not be used.
    public func free() {
        if ctx != nil {
            gs1_encoder_free(ctx)
            ctx = nil
        }
    }

    /// Get the version string of the library.
    ///
    /// Returns a string containing the version of the library, typically the build date.
    ///
    /// - Returns: The version of the library
    public func getVersion() -> String {
        return String(cString: gs1_encoder_getVersion())
    }

    /// Get the current symbology type.
    ///
    /// This might be set manually via `setSym(_:)` or automatically when
    /// processing scan data with `setScanData(_:)`.
    ///
    /// - Returns: The current symbology type
    /// - SeeAlso: `setSym(_:)`
    /// - SeeAlso: `Symbology`
    /// - SeeAlso: `setScanData(_:)`
    public func getSym() -> Symbology {
        return Symbology(rawValue: gs1_encoder_getSym(ctx).rawValue) ?? Symbology.NONE
    }

    /// Set the symbology type.
    ///
    /// This allows the symbology to be specified as any one of the `Symbology` types.
    ///
    /// - Parameter value: The symbology type to set
    /// - Throws: `GS1EncoderError.parameterError` if an invalid symbology type is provided
    /// - SeeAlso: `getSym()`
    /// - SeeAlso: `Symbology`
    public func setSym(_ value: Symbology) throws {
        if !gs1_encoder_setSym(ctx, gs1_encoder_symbologies(value.rawValue)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the current status of the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
    ///
    /// - Returns: `true` if check digits will be generated automatically; `false` if the data must include a valid check digit
    /// - SeeAlso: `setAddCheckDigit(_:)`
    public func getAddCheckDigit() -> Bool {
        return gs1_encoder_getAddCheckDigit(ctx)
    }

    /// Enable or disable "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
    ///
    /// If `false` (default), then the data string must contain a valid check digit.
    /// If `true`, then the data string must not contain a check digit as one will
    /// be generated automatically.
    ///
    /// This option is only valid for symbologies that accept fixed-length data,
    /// specifically EAN/UPC and GS1 DataBar except Expanded (Stacked).
    ///
    /// - Parameter value: `true` to enable automatic check digit generation; `false` to require check digit in data
    /// - Throws: `GS1EncoderError.parameterError` if the value is invalid
    /// - SeeAlso: `getAddCheckDigit()`
    public func setAddCheckDigit(_ value: Bool) throws {
        if !gs1_encoder_setAddCheckDigit(ctx, value) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the status of the "include data titles in HRI" flag.
    ///
    /// Returns `true` if data titles should be included in HRI; otherwise `false`.
    ///
    /// - Returns: The status of the "include data titles in HRI" flag
    public func getIncludeDataTitlesInHRI() -> Bool {
        return gs1_encoder_getIncludeDataTitlesInHRI(ctx)
    }

    /// Set the "include data titles in HRI" flag.
    ///
    /// When set to `true`, data titles from the GS1 General Specification will be
    /// included in the HRI text.
    ///
    /// Default: `false`
    ///
    /// - Parameter value: `true` to include data titles in HRI; `false` otherwise
    /// - Throws: `GS1EncoderError.parameterError` if an error occurs
    public func setIncludeDataTitlesInHRI(_ value: Bool) throws {
        if !gs1_encoder_setIncludeDataTitlesInHRI(ctx, value) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the status of the "permit unknown AIs" mode.
    ///
    /// Returns `true` if unknown AIs are permitted; otherwise `false`.
    ///
    /// - Returns: The status of the "permit unknown AIs" mode
    public func getPermitUnknownAIs() -> Bool {
        return gs1_encoder_getPermitUnknownAIs(ctx)
    }

    /// Set the "permit unknown AIs" mode.
    ///
    /// When set to `true`, AIs that are not recognised by the library will not be rejected.
    /// This can be useful when processing AIs that are not yet supported by the library.
    ///
    /// Default: `false`
    ///
    /// - Parameter value: `true` to permit unknown AIs; `false` to reject them
    /// - Throws: `GS1EncoderError.parameterError` if an error occurs
    public func setPermitUnknownAIs(_ value: Bool) throws {
        if !gs1_encoder_setPermitUnknownAIs(ctx, value) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
    ///
    /// Returns `true` if zero-suppressed GTINs are permitted in Digital Link URIs; otherwise `false`.
    ///
    /// - Returns: The status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode
    public func getPermitZeroSuppressedGTINinDLuris() -> Bool {
        return gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx)
    }

    /// Set the status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
    ///
    /// When set to `true`, zero-suppressed GTINs (such as GTIN-8, GTIN-12, GTIN-13)
    /// will be permitted in Digital Link URIs. When `false`, GTINs must be expressed
    /// in their canonical GTIN-14 form.
    ///
    /// Default: `false`
    ///
    /// - Parameter value: `true` to permit zero-suppressed GTINs; `false` to require canonical form
    /// - Throws: `GS1EncoderError.parameterError` if an error occurs
    public func setPermitZeroSuppressedGTINinDLuris(_ value: Bool) throws {
        if !gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, value) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get whether a specific validation procedure is enabled.
    ///
    /// - Parameter validation: The validation procedure to query
    /// - Returns: `true` if the validation is enabled; `false` otherwise
    /// - SeeAlso: `setValidationEnabled(validation:enabled:)`
    /// - SeeAlso: `Validation`
    public func getValidationEnabled(_ validation: Validation) -> Bool {
        return gs1_encoder_getValidationEnabled(ctx, gs1_encoder_validations(validation.rawValue))
    }

    /// Enable or disable a specific validation procedure.
    ///
    /// Validation procedures can be selectively enabled or disabled to control
    /// which checks are applied to AI data.
    ///
    /// - Parameters:
    ///   - validation: The validation procedure to configure
    ///   - enabled: `true` to enable the validation; `false` to disable it
    /// - Throws: `GS1EncoderError.parameterError` if the validation cannot be changed (e.g., it is locked)
    /// - SeeAlso: `getValidationEnabled(_:)`
    /// - SeeAlso: `Validation`
    public func setValidationEnabled(validation: Validation, enabled: Bool) throws {
        if !gs1_encoder_setValidationEnabled(ctx, gs1_encoder_validations(validation.rawValue), enabled) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the "validate AI associations" flag.
    ///
    /// Returns `true` if mandatory associations between AIs are being validated; otherwise `false`.
    ///
    /// - Returns: The status of the "validate AI associations" flag
    @available(*, deprecated, message: "Use getValidationEnabled(_:) with Validation.RequisiteAIs instead")
    public func getValidateAIassociations() -> Bool {
        return gs1_encoder_getValidateAIassociations(ctx)
    }

    /// Set the "validate AI associations" flag.
    ///
    /// When set to `true`, the library will check for mandatory associations between AIs.
    ///
    /// - Parameter value: `true` to enable validation of AI associations; `false` to disable
    /// - Throws: `GS1EncoderError.parameterError` if an error occurs
    @available(*, deprecated, message: "Use setValidationEnabled(validation:enabled:) with Validation.RequisiteAIs instead")
    public func setValidateAIassociations(_ value: Bool) throws {
        if !gs1_encoder_setValidateAIassociations(ctx, value) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the raw barcode message data.
    ///
    /// The data string represents the raw message that would be encoded in a barcode symbol,
    /// using a harmonised format where FNC1 characters are represented by `^`.
    ///
    /// - Returns: The barcode message data
    /// - SeeAlso: `setDataStr(_:)`
    public func getDataStr() -> String {
        return String(cString: gs1_encoder_getDataStr(ctx))
    }

    /// Set the raw barcode message data.
    ///
    /// This accepts barcode message data in a harmonised format where:
    /// - A leading `^` indicates GS1 Application Identifier syntax data
    /// - A `^` at any other position represents an FNC1 separator character
    ///
    /// - Parameter value: The barcode message data to set
    /// - Throws: `GS1EncoderError.parameterError` if the data is invalid
    /// - SeeAlso: `getDataStr()`
    public func setDataStr(_ value: String) throws {
        if !gs1_encoder_setDataStr(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the AI data as a bracketed AI element string.
    ///
    /// Returns the AI data in human-friendly format using numerical AIs in brackets,
    /// e.g., `(01)12345678901231(10)ABC123`.
    ///
    /// - Returns: The bracketed AI element string, or `nil` if no AI data is available
    /// - SeeAlso: `setAIdataStr(_:)`
    public func getAIdataStr() -> String? {
        let cstr = gs1_encoder_getAIdataStr(ctx)
        if cstr == nil {
            return nil
        }
        return String(cString: cstr!)
    }

    /// Set the AI data using a bracketed AI element string.
    ///
    /// This accepts AI data in human-friendly format using numerical AIs in brackets,
    /// e.g., `(01)12345678901231(10)ABC123`.
    ///
    /// - Parameter value: The bracketed AI element string
    /// - Throws: `GS1EncoderError.parameterError` if the data is invalid
    /// - SeeAlso: `getAIdataStr()`
    public func setAIdataStr(_ value: String) throws {
        if !gs1_encoder_setAIdataStr(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String)) {
            throw GS1EncoderError.parameterError(msg: self.getErrMsg())
        }
    }

    /// Get the scan data.
    ///
    /// Returns the scan data in the format that would be output by a barcode reader
    /// with AIM Symbology Identifiers enabled.
    ///
    /// - Returns: The scan data
    /// - SeeAlso: `setScanData(_:)`
    public func getScanData() -> String? {
        guard let ptr = gs1_encoder_getScanData(ctx) else {
            return nil
        }
        return String(cString: ptr)
    }

    /// Set the scan data.
    ///
    /// This accepts scan data in the format that would be output by a barcode reader
    /// with AIM Symbology Identifiers enabled. The symbology is extracted from the
    /// AIM Symbology Identifier and the AI data is extracted from the message.
    ///
    /// - Parameter value: The scan data to process
    /// - Throws: `GS1EncoderError.scanDataError` if the scan data is invalid
    /// - Note: Requires that AIM Symbology Identifiers are enabled on the barcode reader,
    ///   and that any embedded FNC1 separator characters are represented by GS characters (ASCII 29)
    /// - SeeAlso: `getScanData()`
    /// - SeeAlso: `getSym()`
    public func setScanData(_ value: String) throws {
        if !gs1_encoder_setScanData(ctx, UnsafeMutablePointer<CChar>(mutating: (value as NSString).utf8String)) {
            throw GS1EncoderError.scanDataError(msg: self.getErrMsg())
        }
    }

    /// Get the error markup generated when parsing AI data fails due to a linting failure.
    ///
    /// When a setter function throws an error due to invalid AI-based data, a marked up
    /// instance of the AI that failed will be generated. Where it is meaningful to identify
    /// offending characters in the input data, these characters will be surrounded by `|`
    /// characters. Otherwise the entire AI value will be surrounded by `|` characters.
    ///
    /// - Returns: The error markup showing the location of the error in the input data
    public func getErrMarkup() -> String {
        return String(cString: gs1_encoder_getErrMarkup(ctx))
    }

    /// Get a GS1 Digital Link URI that represents the AI-based input data.
    ///
    /// This generates a GS1 Digital Link URI from the current AI data. An optional
    /// stem can be provided to specify the domain and path prefix for the URI.
    ///
    /// - Parameter stem: Optional URI stem (e.g., `https://example.com/`). If not provided, a default stem is used
    /// - Returns: The GS1 Digital Link URI
    /// - Throws: `GS1EncoderError.digitalLinkError` if the Digital Link URI cannot be generated
    /// - Note: AI data must be set using `setAIdataStr(_:)`, `setDataStr(_:)` or `setScanData(_:)` before calling this method
    public func getDLuri(_ stem: String? = nil) throws -> String {
        var cstr: UnsafeMutablePointer<CChar>?
        if let ustem = stem {
            cstr = gs1_encoder_getDLuri(ctx, UnsafeMutablePointer<CChar>(mutating: (ustem as NSString).utf8String))
        } else {
            cstr = gs1_encoder_getDLuri(ctx, nil)
        }
        if cstr == nil {
            throw GS1EncoderError.digitalLinkError(msg: self.getErrMsg())
        }
        return String(cString: cstr!)
    }

    /// Get the Human-Readable Interpretation (HRI) text for the current AI data as an array of strings.
    ///
    /// The HRI provides a human-friendly representation of the AI data, with each array
    /// element representing a separate line of text. If data titles are enabled via
    /// `setIncludeDataTitlesInHRI(_:)`, they will be included in the output.
    ///
    /// - Returns: Array of strings representing the HRI text lines
    /// - SeeAlso: `setIncludeDataTitlesInHRI(_:)`
    public func getHRI() -> Array<String> {
        let hri = UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<CChar>?>?>.allocate(capacity: 1)
        defer {
            hri.deinitialize(count: 1)
            hri.deallocate()
        }
        let numHRI = Int(gs1_encoder_getHRI(ctx, hri))
        var out = [String]()
        for (_, value) in UnsafeBufferPointer(start: hri.pointee, count: numHRI).enumerated() {
            out.append(String(cString: value!))
        }
        return out
    }

    /// Get the non-numeric (ignored) query parameters from a GS1 Digital Link URI as an array of strings.
    ///
    /// When a GS1 Digital Link URI is processed via `setScanData(_:)`, any non-numeric
    /// query parameters (those not representing AI data) are extracted and made available
    /// through this method.
    ///
    /// - Returns: Array of strings representing the ignored query parameters
    /// - SeeAlso: `setScanData(_:)`
    public func getDLignoredQueryParams() -> Array<String> {
        let qp = UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<CChar>?>?>.allocate(capacity: 1)
        defer {
            qp.deinitialize(count: 1)
            qp.deallocate()
        }
        let numQP = Int(gs1_encoder_getDLignoredQueryParams(ctx, qp))
        var out = [String]()
        for (_, value) in UnsafeBufferPointer(start: qp.pointee, count: numQP).enumerated() {
            out.append(String(cString: value!))
        }
        return out
    }

}

/// Custom error types for GS1 Encoder operations.
///
/// These errors are thrown by GS1Encoder methods when operations fail.
public enum GS1EncoderError: Error {
    /// A general error occurred
    case generalError(msg: String)

    /// An invalid parameter was provided
    case parameterError(msg: String)

    /// Scan data processing failed
    case scanDataError(msg: String)

    /// GS1 Digital Link URI generation or processing failed
    case digitalLinkError(msg: String)
}
