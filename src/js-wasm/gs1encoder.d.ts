/**
 * Main class for processing GS1 barcode data, including validation, format conversion, and generation of outputs such as GS1 Digital Link URIs and Human-Readable Interpretation text.
 */
export class GS1encoder {
    /**
     * @private
     */
    private ctx;
    /**
     * Initialises a new instance of the GS1Encoder.
     *
     * @returns {Promise<void>}
     * @throws {GS1encoderGeneralException} if the library fails to initialise
     * @async
     */
    init(): Promise<void>;
    /**
     *  Load the WASM
     *  @private
     */
    private module;
    /**
     *  Public API functions implemented by the WASM build of the GS1
     *  Syntax Engine library
     *  @private
     */
    private api;
    /**
     * Frees the resources associated with this encoder instance.
     * @returns {void}
     */
    free(): void;
    /**
     * Get the version string of the library.
     * <p>
     * Returns a string containing the version of the library, typically the build date.
     *
     * @type {string}
     * @returns {string}
     */
    get version(): string;
    /**
     * Get the error markup generated when parsing AI data fails due to a linting failure.
     * <p>
     * When a setter function returns <code>false</code> (indicating an error), if that failure is due to
     * AI-based data being invalid, a marked up instance of the AI that failed will be generated.
     * <p>
     * Where it is meaningful to identify offending characters in the input data, these characters
     * will be surrounded by <code>"|"</code> characters. Otherwise the entire AI value will be surrounded by
     * <code>"|"</code> characters.
     *
     * @type {string}
     * @returns {string}
     */
    get errMarkup(): string;
    /**
     * @param {Symbology} value
     */
    set sym(value: Symbology);
    /**
     * Get/set the symbology type.
     * <p>
     * This might be set manually or automatically when processing scan data with {@link GS1encoder#scanData}.
     *
     * @type {Symbology}
     * @returns {Symbology}
     * @throws {GS1encoderParameterException} if the setter is provided with an invalid symbology type
     * @see {@link GS1encoder#scanData}
     * @see GS1encoder.symbology
     */
    get sym(): Symbology;
    /**
     * @param {boolean} value
     */
    set addCheckDigit(value: boolean);
    /**
     * Get/set the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     * <p>
     * If <code>false</code> (default), then the data string must contain a valid check digit.
     * If <code>true</code>, then the data string must not contain a check digit as one will
     * be generated automatically.
     * <p>
     * This option is only valid for symbologies that accept fixed-length data,
     * specifically EAN/UPC and GS1 DataBar except Expanded (Stacked).
     *
     * @type {boolean}
     * @returns {boolean}
     * @throws {GS1encoderParameterException} if the value is invalid
     */
    get addCheckDigit(): boolean;
    set permitUnknownAIs(value: boolean);
    /**
     * Get/set the "permit unknown AIs" mode.
     * <p>
     * If <code>false</code> (default), then all AIs represented by the input data must be
     * known.
     * <p>
     * If <code>true</code>, then unknown AIs (those not in this library's static AI table)
     * will be accepted.
     * <p>
     * <strong>Note:</strong> The option only applies to parsed input data, specifically bracketed AI data
     * supplied with {@link GS1encoder#aiDataStr} and GS1 Digital Link URIs supplied
     * with {@link GS1encoder#dataStr}. Unbracketed AI element strings containing
     * unknown AIs cannot be parsed because it is not possible to differentiate the
     * AI from its data value when the length of the AI is uncertain.
     * <p>
     * Default: <code>false</code>
     *
     * @type {boolean}
     * @throws {@link GS1encoderParameterException}
     */
    get permitUnknownAIs(): boolean;
    set permitZeroSuppressedGTINinDLuris(value: boolean);
    /**
     * Get/set the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
     * <p>
     * If false (default), then the value of a path component for AI (01) must
     * be provided as a full GTIN-14.
     * <p>
     * If true, then the value of a path component for AI (01) may contain the
     * GTIN-14 with zeros suppressed, in the format of a GTIN-13, GTIN-12 or GTIN-8.
     * <p>
     * This option only applies to parsed input data, specifically GS1 Digital Link
     * URIs. Since zero-suppressed GTINs are deprecated, this option should only be
     * enabled when it is necessary to accept legacy GS1 Digital Link URIs having
     * zero-suppressed GTIN-14.
     * <p>
     * Default: false
     *
     * @type {boolean}
     * @throws {@link GS1encoderParameterException}
     */
    get permitZeroSuppressedGTINinDLuris(): boolean;
    set includeDataTitlesInHRI(value: boolean);
    /**
     * Get/set the "include data titles in HRI" flag.
     * <p>
     * When set to <code>true</code>, data titles from the GS1 General Specification will be
     * included in the HRI text.
     * <p>
     * Default: <code>false</code>
     *
     * @type {boolean}
     * @throws {@link GS1encoderParameterException}
     */
    get includeDataTitlesInHRI(): boolean;
    /**
     * Get the current enabled status of the provided AI validation procedure.
     *
     * @param {Validation} validation - A validation procedure to check the status of
     * @returns {boolean} <code>true</code> if the AI validation procedure is currently enabled; <code>false</code> otherwise
     */
    getValidationEnabled(validation: Validation): boolean;
    /**
     * Enable or disable the given AI validation procedure.
     * <p>
     * This determines whether certain checks are enforced when data is provided using
     * {@link GS1encoder#aiDataStr}, {@link GS1encoder#dataStr} or {@link GS1encoder#scanData}.
     * <p>
     * If enabled is <code>true</code> (default), then the corresponding validation will be enforced.
     * If enabled is <code>false</code>, then the corresponding validation will not be enforced.
     * <p>
     * <strong>Note:</strong> The option only applies to AI input data.
     *
     * @param {Validation} validation - A validation procedure to set the enabled status of
     * @param {boolean} value - <code>true</code> to enable the validation; <code>false</code> to disable
     * @returns {void}
     * @throws {GS1encoderParameterException}
     */
    setValidationEnabled(validation: Validation, value: boolean): void;
    set validateAIassociations(value: boolean);
    /**
     * Get/set the "validate AI associations" flag.
     * <p>
     * <strong>Deprecated:</strong> Use {@link GS1encoder#getValidationEnabled} and
     * {@link GS1encoder#setValidationEnabled} instead.
     * <p>
     * This property is equivalent to using the {@link GS1encoder#getValidationEnabled} and
     * {@link GS1encoder#setValidationEnabled} methods with the
     * {@link GS1encoder.validation RequisiteAIs} validation procedure.
     *
     * @type {boolean}
     * @deprecated Use {@link GS1encoder#getValidationEnabled} and {@link GS1encoder#setValidationEnabled} instead
     * @throws {@link GS1encoderParameterException}
     */
    get validateAIassociations(): boolean;
    set aiDataStr(value: string);
    /**
     * Get/set the barcode data input buffer using GS1 Application Identifier syntax.
     * <p>
     * The input is provided in human-friendly format <strong>without</strong> FNC1 characters
     * which are inserted automatically, for example:
     * <p>
     * <pre>(01)12345678901231(10)ABC123(11)210630</pre>
     * <p>
     * This syntax harmonises the format for the input accepted by all symbologies.
     * For example, the following input is acceptable for EAN-13, UPC-A, UPC-E, any
     * variant of the GS1 DataBar family, GS1 QR Code and GS1 DataMatrix:
     * <p>
     * <pre>(01)00031234000054</pre>
     * <p>
     * The input is immediately parsed and validated against certain rules for GS1 AIs, after
     * which the resulting encoding for valid inputs is available via {@link GS1encoder#dataStr}.
     * If the input is invalid then an exception will be thrown.
     * <p>
     * Any <code>"("</code> characters in AI element values must be escaped as <code>"\\("</code> to avoid
     * conflating them with the start of the next AI.
     * <p>
     * For symbologies that support a composite component (all except Data Matrix, QR Code,
     * and DotCode), the data for the linear and 2D components can be separated by a
     * <code>"|"</code> character, for example:
     * <p>
     * <pre>(01)12345678901231|(10)ABC123(11)210630</pre>
     *
     * @type {string}
     * @throws {@link GS1encoderParameterException}
     */
    get aiDataStr(): string;
    set dataStr(value: string);
    /**
     * Get/set the raw data that would be directly encoded within a GS1 barcode message.
     * <p>
     * A <code>"^"</code> character at the start of the input indicates that the data is in GS1
     * Application Identifier syntax. In this case, all subsequent instances of the
     * <code>"^"</code> character represent the FNC1 non-data characters that are used to
     * separate fields that are not specified as being pre-defined length from
     * subsequent fields.
     * <p>
     * Inputs beginning with <code>"^"</code> will be validated against certain data syntax
     * rules for GS1 AIs. If the input is invalid then the setter will throw
     * a {@link GS1encoderParameterException}. In the case that the data is
     * unacceptable due to invalid AI content then a marked up version of the
     * offending AI can be retrieved using {@link GS1encoder#errMarkup}.
     * <p>
     * <strong>Note:</strong> It is strongly advised that GS1 data input is instead specified using
     * {@link GS1encoder#aiDataStr} which takes care of the AI encoding rules
     * automatically, including insertion of FNC1 characters where required. This
     * can be used for all symbologies that accept GS1 AI syntax data.
     * <p>
     * Inputs beginning with <code>"http://"</code> or <code>"https://"</code> will be parsed as a GS1
     * Digital Link URI during which the corresponding AI element string is
     * extracted and validated.
     * <p>
     * EAN/UPC, GS1 DataBar and GS1-128 support a Composite Component. The
     * Composite Component must be specified in AI syntax. It must be separated
     * from the primary linear components with a <code>"|"</code> character and begin with an
     * FNC1 in first position, for example:
     * <pre>
     * encoder.dataStr = "^0112345678901231|^10ABC123^11210630";
     * </pre>
     * <p>
     * The above specifies a linear component representing "(01)12345678901231"
     * together with a composite component representing "(10)ABC123(11)210630".
     * <p>
     * <strong>Note:</strong> For GS1 data it is simpler and less error prone to specify the input
     * in human-friendly GS1 AI syntax using {@link GS1encoder#aiDataStr}.
     *
     * @type {string}
     * @throws {@link GS1encoderParameterException} if the setter is provided with invalid data
     * @see {@link GS1encoder#aiDataStr}
     * @see {@link GS1encoder#errMarkup}
     */
    get dataStr(): string;
    /**
     * Get a GS1 Digital Link URI that represents the AI-based input data.
     * <p>
     * This method converts AI-based input data into a GS1 Digital Link URI format.
     * <p>
     * Example: <pre>(01)12345678901231(10)ABC123(11)210630</pre> with stem
     * <code>https://id.example.com/stem</code> might produce:
     * <pre>https://id.example.com/stem/01/12345678901231?10=ABC123&11=210630</pre>
     *
     * @param {string|null} stem - A URI "stem" used as a prefix for the URI. If <code>null</code>, the GS1 canonical stem (https://id.gs1.org/) will be used
     * @returns {string} a string representing the GS1 Digital Link URI for the input data
     * @throws {GS1encoderDigitalLinkException}
     */
    getDLuri(stem: string | null): string;
    set scanData(value: string);
    /**
     * Process scan data received from a barcode reader or return the expected scan data string.
     * <p>
     * <strong>Setting:</strong> Process normalised scan data received from a barcode reader with
     * reporting of AIM symbology identifiers enabled to extract the message data and perform
     * syntax checks in the case of GS1 Digital Link and AI data input.
     * <p>
     * This function will process scan data (such as the output of a barcode reader) and process
     * the received data, setting the data input buffer to the message received and setting the
     * selected symbology to something that is able to carry the received data.
     * <p>
     * <strong>Note:</strong> In some instances the symbology determined by this library will not match
     * that of the image that was scanned. The AIM symbology identifier prefix of the
     * scan data does not always uniquely identify the symbology that was scanned.
     * For example GS1-128 Composite symbols share the same symbology identifier as
     * the GS1 DataBar family, and will therefore be detected as such.
     * <p>
     * A literal <code>"|"</code> character may be included in the scan data to indicate the
     * separation between the first and second messages that would be transmitted
     * by a reader that is configured to return the composite component when
     * reading EAN/UPC symbols.
     * <p>
     * Example scan data input: <pre>]C1011231231231233310ABC123{GS}99TESTING</pre>
     * where {GS} represents ASCII character 29.
     * <p>
     * <strong>Getting:</strong> Returns the string that should be returned by scanners when reading a
     * symbol that is an instance of the selected symbology and contains the same input data.
     * <p>
     * The output will be prefixed with the appropriate AIM symbology identifier.
     *
     * @type {string}
     * @throws {@link GS1encoderParameterException}
     */
    get scanData(): string;
    /**
     * Get the Human-Readable Interpretation ("HRI") text for the current data input buffer.
     * <p>
     * For composite symbols, a separator "--" will be included in the array to distinguish
     * between the linear and 2D components.
     * <p>
     * Example output for <code>^011231231231233310ABC123|^99XYZ(TM) CORP</code>:
     * <pre>
     * (01) 12312312312333
     * (10) ABC123
     * --
     * (99) XYZ(TM) CORP
     * </pre>
     *
     * @type {string[]}
     * @returns {string[]}
     */
    get hri(): string[];
    /**
     * Get the non-numeric (ignored) query parameters from a GS1 Digital Link URI.
     * <p>
     * For example, if the input data buffer contains:
     * <pre>https://a/01/12312312312333/22/ABC?name=Donald%2dDuck&amp;99=ABC&amp;testing&amp;type=cartoon</pre>
     * <p>
     * Then this property returns: <code>name=Donald%2dDuck</code>, <code>testing</code>, <code>type=cartoon</code>
     * <p>
     * The returned strings are not URI decoded. The expected use for this property is to
     * present which sections of a given GS1 Digital Link URI have been ignored.
     *
     * @type {string[]}
     * @returns {string[]}
     */
    get dlIgnoredQueryParams(): string[];
}
export namespace GS1encoder {
    export { symbology };
    export { validation };
}
/**
 * - Numeric symbology identifier
 */
export type Symbology = number;
/**
 * - Numeric validation identifier
 */
export type Validation = number;
export type SymbologyEnum = {
    /**
     * None defined
     */
    NONE: Symbology;
    /**
     * GS1 DataBar Omnidirectional
     */
    DataBarOmni: Symbology;
    /**
     * GS1 DataBar Truncated
     */
    DataBarTruncated: Symbology;
    /**
     * GS1 DataBar Stacked
     */
    DataBarStacked: Symbology;
    /**
     * GS1 DataBar Stacked Omnidirectional
     */
    DataBarStackedOmni: Symbology;
    /**
     * GS1 DataBar Limited
     */
    DataBarLimited: Symbology;
    /**
     * GS1 DataBar Expanded (Stacked)
     */
    DataBarExpanded: Symbology;
    /**
     * UPC-A
     */
    UPCA: Symbology;
    /**
     * UPC-E
     */
    UPCE: Symbology;
    /**
     * EAN-13
     */
    EAN13: Symbology;
    /**
     * EAN-8
     */
    EAN8: Symbology;
    /**
     * GS1-128 with CC-A or CC-B
     */
    GS1_128_CCA: Symbology;
    /**
     * GS1-128 with CC-C
     */
    GS1_128_CCC: Symbology;
    /**
     * (GS1) QR Code
     */
    QR: Symbology;
    /**
     * (GS1) Data Matrix
     */
    DM: Symbology;
    /**
     * (GS1) DotCode
     */
    DotCode: Symbology;
    /**
     * Value is the number of symbologies
     */
    NUMSYMS: Symbology;
};
export type ValidationEnum = {
    /**
     * Mutually exclusive AIs (locked: always enabled)
     */
    MutexAIs: Validation;
    /**
     * Mandatory associations between AIs
     */
    RequisiteAIs: Validation;
    /**
     * Repeated AIs having same value (locked: always enabled)
     */
    RepeatedAIs: Validation;
    /**
     * Serialisation qualifier AIs must be present with Digital Signature (locked: always enabled)
     */
    DigSigSerialKey: Validation;
    /**
     * Unknown AIs not permitted as GS1 DL URI data attributes
     */
    UnknownAInotDLattr: Validation;
    /**
     * Value is the number of validations
     */
    NUMVALIDATIONS: Validation;
};
/**
 * Recognised GS1 barcode formats ("symbologies") for processing scan data.
 * <p>
 * This object defines all supported GS1 barcode symbology types that can be used
 * with the encoder. Each symbology has specific characteristics and use cases.
 *
 * @typedef {number} Symbology - Numeric symbology identifier
 */
/**
 * @typedef {object} SymbologyEnum
 * @property {Symbology} NONE None defined
 * @property {Symbology} DataBarOmni GS1 DataBar Omnidirectional
 * @property {Symbology} DataBarTruncated GS1 DataBar Truncated
 * @property {Symbology} DataBarStacked GS1 DataBar Stacked
 * @property {Symbology} DataBarStackedOmni GS1 DataBar Stacked Omnidirectional
 * @property {Symbology} DataBarLimited GS1 DataBar Limited
 * @property {Symbology} DataBarExpanded GS1 DataBar Expanded (Stacked)
 * @property {Symbology} UPCA UPC-A
 * @property {Symbology} UPCE UPC-E
 * @property {Symbology} EAN13 EAN-13
 * @property {Symbology} EAN8 EAN-8
 * @property {Symbology} GS1_128_CCA GS1-128 with CC-A or CC-B
 * @property {Symbology} GS1_128_CCC GS1-128 with CC-C
 * @property {Symbology} QR (GS1) QR Code
 * @property {Symbology} DM (GS1) Data Matrix
 * @property {Symbology} DotCode (GS1) DotCode
 * @property {Symbology} NUMSYMS Value is the number of symbologies
 * @readonly
 */
/** @type {SymbologyEnum} */
declare const symbology: SymbologyEnum;
/**
 * Optional AI validation procedures that may be applied to detect invalid inputs.
 * <p>
 * These validation procedures are applied when AI data is provided using
 * {@link GS1encoder#aiDataStr}, {@link GS1encoder#dataStr} or {@link GS1encoder#scanData}.
 * <p>
 * Only AI validation procedures whose "enabled" status can be updated (i.e. not "locked") are described.
 *
 * @typedef {number} Validation - Numeric validation identifier
 */
/**
 * @typedef {object} ValidationEnum
 * @property {Validation} MutexAIs Mutually exclusive AIs (locked: always enabled)
 * @property {Validation} RequisiteAIs Mandatory associations between AIs
 * @property {Validation} RepeatedAIs Repeated AIs having same value (locked: always enabled)
 * @property {Validation} DigSigSerialKey Serialisation qualifier AIs must be present with Digital Signature (locked: always enabled)
 * @property {Validation} UnknownAInotDLattr Unknown AIs not permitted as GS1 DL URI data attributes
 * @property {Validation} NUMVALIDATIONS Value is the number of validations
 * @readonly
 */
/** @type {ValidationEnum} */
declare const validation: ValidationEnum;
/**
 * Exception thrown when a general library error occurs, such as initialisation failure.
 * @class
 * @extends Error
 * @param {string} message - The error message
 */
export function GS1encoderGeneralException(message: string): Error;
export class GS1encoderGeneralException {
    /**
     * Exception thrown when a general library error occurs, such as initialisation failure.
     * @class
     * @extends Error
     * @param {string} message - The error message
     */
    constructor(message: string);
    name: string;
}
/**
 * Exception thrown when an invalid parameter is provided to a method or property setter.
 * @class
 * @extends Error
 * @param {string} message - The error message
 */
export function GS1encoderParameterException(message: string): Error;
export class GS1encoderParameterException {
    /**
     * Exception thrown when an invalid parameter is provided to a method or property setter.
     * @class
     * @extends Error
     * @param {string} message - The error message
     */
    constructor(message: string);
    name: string;
}
/**
 * Exception thrown when an error occurs during GS1 Digital Link URI processing.
 * @class
 * @extends Error
 * @param {string} message - The error message
 */
export function GS1encoderDigitalLinkException(message: string): Error;
export class GS1encoderDigitalLinkException {
    /**
     * Exception thrown when an error occurs during GS1 Digital Link URI processing.
     * @class
     * @extends Error
     * @param {string} message - The error message
     */
    constructor(message: string);
    name: string;
}
/**
 * Exception thrown when an error occurs during scan data processing.
 * @class
 * @extends Error
 * @param {string} message - The error message
 */
export function GS1encoderScanDataException(message: string): Error;
export class GS1encoderScanDataException {
    /**
     * Exception thrown when an error occurs during scan data processing.
     * @class
     * @extends Error
     * @param {string} message - The error message
     */
    constructor(message: string);
    name: string;
}
export { symbology as Symbology, validation as Validation };
