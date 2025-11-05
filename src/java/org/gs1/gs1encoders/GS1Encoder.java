package org.gs1.gs1encoders;

/*
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
 */

/**
 * Main class for processing GS1 barcode data, including validation, format conversion, and generation of outputs such as GS1 Digital Link URIs and Human-Readable Interpretation text.
 */
public class GS1Encoder {

    /*
     *  Functions imported from the JNI interface
     *
     */
    private static native String gs1encoderGetErrMsgJNI(long ctx);
    private static native long gs1encoderInitJNI();
    private static native void gs1encoderFreeJNI(long ctx);
    private static native String gs1encoderGetVersionJNI();
    private static native int gs1encoderGetSymJNI(long ctx);
    private static native boolean gs1encoderSetSymJNI(long ctx, int value);
    private static native boolean gs1encoderGetAddCheckDigitJNI(long ctx);
    private static native boolean gs1encoderSetAddCheckDigitJNI(long ctx, boolean value);
    private static native boolean gs1encoderGetIncludeDataTitlesInHRIJNI(long ctx);
    private static native boolean gs1encoderSetIncludeDataTitlesInHRIJNI(long ctx, boolean value);
    private static native boolean gs1encoderGetPermitUnknownAIsJNI(long ctx);
    private static native boolean gs1encoderSetPermitUnknownAIsJNI(long ctx, boolean value);
    private static native boolean gs1encoderGetPermitZeroSuppressedGTINinDLurisJNI(long ctx);
    private static native boolean gs1encoderSetPermitZeroSuppressedGTINinDLurisJNI(long ctx, boolean value);
    private static native boolean gs1encoderGetValidationEnabledJNI(long ctx, int validation);
    private static native boolean gs1encoderSetValidationEnabledJNI(long ctx, int validation, boolean value);
    private static native boolean gs1encoderGetValidateAIassociationsJNI(long ctx);
    private static native boolean gs1encoderSetValidateAIassociationsJNI(long ctx, boolean value);
    private static native String gs1encoderGetDataStrJNI(long ctx);
    private static native boolean gs1encoderSetDataStrJNI(long ctx, String value);
    private static native String gs1encoderGetAIdataStrJNI(long ctx);
    private static native boolean gs1encoderSetAIdataStrJNI(long ctx, String value);
    private static native String gs1encoderGetScanDataJNI(long ctx);
    private static native boolean gs1encoderSetScanDataJNI(long ctx, String value);
    private static native String gs1encoderGetErrMarkupJNI(long ctx);
    private static native String gs1encoderGetDLuriJNI(long ctx, String stem);
    private static native String[] gs1encoderGetHRIJNI(long ctx);
    private static native String[] gs1encoderGetDLignoredQueryParamsJNI(long ctx);

    // Initialisation loads the gs1encoders JNI library interface
    static {
        System.loadLibrary("gs1encoders");
    }

    /**
     * Recognised GS1 barcode formats ("symbologies") for processing scan data.
     * <p>
     * This enum defines all supported GS1 barcode symbology types that can be used
     * with the encoder. Each symbology has specific characteristics and use cases.
     */
    public enum Symbology {
            /**
             * None defined
             */
            NONE,

            /**
             * GS1 DataBar Omnidirectional
             */
            DataBarOmni,

            /**
             * GS1 DataBar Truncated
             */
            DataBarTruncated,

            /**
             * GS1 DataBar Stacked
             */
            DataBarStacked,

            /**
             * GS1 DataBar Stacked Omnidirectional
             */
            DataBarStackedOmni,

            /**
             * GS1 DataBar Limited
             */
            DataBarLimited,

            /**
             * GS1 DataBar Expanded (Stacked)
             */
            DataBarExpanded,

            /**
             * UPC-A
             */
            UPCA,

            /**
             * UPC-E
             */
            UPCE,

            /**
             * EAN-13
             */
            EAN13,

            /**
             * EAN-8
             */
            EAN8,

            /**
             * GS1-128 with CC-A or CC-B
             */
            GS1_128_CCA,

            /**
             * GS1-128 with CC
             */
            GS1_128_CCC,

            /**
             * (GS1) QR Code
             */
            QR,

            /**
             * (GS1) Data Matrix
             */
            DM,

            /**
             * (GS1) DotCode
             */
            DotCode,

            /**
             * Value is the number of symbologies
             */
            NUMSYMS
    }


    /**
     * Optional AI validation procedures that may be applied to detect invalid inputs.
     * <p>
     * These validation procedures are applied when AI data is provided using
     * {@link #setAIdataStr(String)}, {@link #setDataStr(String)} or {@link #setScanData(String)}.
     * <p>
     * <strong>Note:</strong> Some validation procedures are "locked" (always enabled and cannot be modified).
     * All validation procedures are listed to maintain correct enum value alignment with the native library.
     */
    public enum Validation {
            /**
             * Mutually exclusive AIs (locked: always enabled)
             */
            MutexAIs,

            /**
             * Mandatory associations between AIs
             */
            RequisiteAIs,

            /**
             * Repeated AIs having same value (locked: always enabled)
             */
            RepeatedAIs,

            /**
             * Serialisation qualifier AIs must be present with Digital Signature (locked: always enabled)
             */
            DigSigSerialKey,

            /**
             * Unknown AIs not permitted as GS1 DL URI data attributes
             */
            UnknownAInotDLattr,

            /**
             * Value is the number of validations
             */
            NUMVALIDATIONS
    }


    /**
     * An opaque pointer used by the native code to represent an
     * "instance" of the library. It is hidden behind the object
     * interface that is provided to users of this wrapper.
     */
    private long ctx;

    /*
     *  Methods to provide a wrapper around the functional interface imported from the native library
     *
     */

    // This Java wrapper library throws an exception containing the error message whenever
    // an error is returned by the native library. Therefore direct access to the native
    // error message is not necessary.
    private String getErrMsg() {
        return gs1encoderGetErrMsgJNI(ctx);
    }


    /**
     * Initialises a new instance of the GS1Encoder class.
     *
     * @throws GS1EncoderGeneralException if the library fails to initialise
     */
    public GS1Encoder() throws GS1EncoderGeneralException {
        ctx = gs1encoderInitJNI();
        if (ctx == 0)
            throw new GS1EncoderGeneralException("Failed to initialise the native library");
    }

    /**
     * Release library resources.
     *
     * @hidden
     */
    public void free() {
        if (ctx != 0)
            gs1encoderFreeJNI(ctx);
        ctx = 0;
    }

    /**
     * Get the version string of the library.
     * <p>
     * Returns a string containing the version of the library, typically the build date.
     *
     * @return the version of the library
     */
    public String getVersion() {
        return gs1encoderGetVersionJNI();
    }

    /**
     * Get the current symbology type.
     * <p>
     * This might be set manually via {@link #setSym(Symbology)} or automatically when
     * processing scan data with {@link #setScanData(String)}.
     *
     * @return the current symbology type
     * @see #setSym(Symbology)
     * @see Symbology
     * @see #setScanData(String)
     */
    public Symbology getSym() {
        return Symbology.values()[gs1encoderGetSymJNI(ctx) + 1];
    }

    /**
     * Set the symbology type.
     * <p>
     * This allows the symbology to be specified as any one of the {@link Symbology} types.
     *
     * @param value the symbology type to set
     * @throws GS1EncoderParameterException if an invalid symbology type is provided
     * @see #getSym()
     * @see Symbology
     */
    public void setSym(Symbology value) throws GS1EncoderParameterException {
        if (!gs1encoderSetSymJNI(ctx, value.ordinal() - 1))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the current status of the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     *
     * @return {@code true} if check digits will be generated automatically; {@code false} if the data must include a valid check digit
     * @see #setAddCheckDigit(boolean)
     */
    public boolean getAddCheckDigit() {
        return gs1encoderGetAddCheckDigitJNI(ctx);
    }

    /**
     * Enable or disable "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     * <p>
     * If {@code false} (default), then the data string must contain a valid check digit.
     * If {@code true}, then the data string must not contain a check digit as one will
     * be generated automatically.
     * <p>
     * This option is only valid for symbologies that accept fixed-length data,
     * specifically EAN/UPC and GS1 DataBar except Expanded (Stacked).
     *
     * @param value {@code true} to enable automatic check digit generation; {@code false} to require check digit in data
     * @throws GS1EncoderParameterException if the value is invalid
     * @see #getAddCheckDigit()
     */
    public void setAddCheckDigit(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetAddCheckDigitJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the status of the "include data titles in HRI" flag.
     * <p>
     * Returns {@code true} if data titles should be included in HRI; otherwise {@code false}.
     *
     * @return the status of the "include data titles in HRI" flag
     */
    public boolean getIncludeDataTitlesInHRI() {
        return gs1encoderGetIncludeDataTitlesInHRIJNI(ctx);
    }

    /**
     * Set the "include data titles in HRI" flag.
     * <p>
     * When set to {@code true}, data titles from the GS1 General Specification will be
     * included in the HRI text.
     * <p>
     * Default: {@code false}
     *
     * @param value {@code true} to include data titles in HRI; {@code false} otherwise
     * @throws GS1EncoderParameterException if an error occurs
     */
    public void setIncludeDataTitlesInHRI(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetIncludeDataTitlesInHRIJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the status of the "permit unknown AIs" mode.
     * <p>
     * Returns {@code true} if unknown AIs are permitted; otherwise {@code false}.
     *
     * @return the status of the "permit unknown AIs" mode
     */
    public boolean getPermitUnknownAIs() {
        return gs1encoderGetPermitUnknownAIsJNI(ctx);
    }

    /**
     * Set the "permit unknown AIs" mode.
     * <p>
     * If {@code false} (default), then all AIs represented by the input data must be
     * known.
     * <p>
     * If {@code true}, then unknown AIs (those not in this library's static AI table)
     * will be accepted.
     * <p>
     * <strong>Note:</strong> The option only applies to parsed input data, specifically bracketed AI data
     * supplied with {@link #setAIdataStr(String)} and GS1 Digital Link URIs supplied
     * with {@link #setDataStr(String)}. Unbracketed AI element strings containing
     * unknown AIs cannot be parsed because it is not possible to differentiate the
     * AI from its data value when the length of the AI is uncertain.
     * <p>
     * Default: {@code false}
     *
     * @param value {@code true} to permit unknown AIs; {@code false} otherwise
     * @throws GS1EncoderParameterException if an error occurs
     */
    public void setPermitUnknownAIs(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetPermitUnknownAIsJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the status of the "permit zero-suppressed GTINs in GS1 DL URIs" mode.
     * <p>
     * Returns {@code true} if zero-suppressed GTINs are permitted in GS1 Digital Link URIs; otherwise {@code false}.
     *
     * @return the status of the "permit zero-suppressed GTINs in GS1 DL URIs" mode
     */
    public boolean getPermitZeroSuppressedGTINinDLuris() {
        return gs1encoderGetPermitZeroSuppressedGTINinDLurisJNI(ctx);
    }

    /**
     * Set the status of the "permit zero-suppressed GTINs in GS1 DL URIs" mode.
     * <p>
     * If {@code false} (default), then the value of a path component for AI (01) must
     * be provided as a full GTIN-14.
     * <p>
     * If {@code true}, then the value of a path component for AI (01) may contain the
     * GTIN-14 with zeros suppressed, in the format of a GTIN-13, GTIN-12 or GTIN-8.
     * <p>
     * This option only applies to parsed input data, specifically GS1 Digital Link
     * URIs. Since zero-suppressed GTINs are deprecated, this option should only be
     * enabled when it is necessary to accept legacy GS1 Digital Link URIs having
     * zero-suppressed GTIN-14.
     * <p>
     * Default: {@code false}
     *
     * @param value {@code true} to permit zero-suppressed GTINs; {@code false} otherwise
     * @throws GS1EncoderParameterException if an error occurs
     */
    public void setPermitZeroSuppressedGTINinDLuris(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetPermitZeroSuppressedGTINinDLurisJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the current enabled status of the provided AI validation procedure.
     *
     * @param validation a validation procedure to check the status of
     * @return {@code true} if the AI validation procedure is currently enabled; {@code false} otherwise
     */
    public boolean getValidationEnabled(Validation validation) {
        return gs1encoderGetValidationEnabledJNI(ctx, validation.ordinal());
    }

    /**
     * Enable or disable the given AI validation procedure.
     * <p>
     * This determines whether certain checks are enforced when data is provided using
     * {@link #setAIdataStr(String)}, {@link #setDataStr(String)} or {@link #setScanData(String)}.
     * <p>
     * If enabled is {@code true} (default), then the corresponding validation will be enforced.
     * If enabled is {@code false}, then the corresponding validation will not be enforced.
     * <p>
     * <strong>Note:</strong> The option only applies to AI input data.
     *
     * @param validation a validation procedure to set the enabled status of
     * @param value {@code true} to enable the validation; {@code false} to disable
     * @throws GS1EncoderParameterException if an error occurs
     */
    public void setValidationEnabled(Validation validation, boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetValidationEnabledJNI(ctx, validation.ordinal(), value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the current enabled status of the {@link Validation#RequisiteAIs} validation procedure.
     * <p>
     * <strong>Deprecated:</strong> Use {@link #getValidationEnabled(Validation)} instead.
     * <p>
     * This method is equivalent to calling {@link #getValidationEnabled(Validation)} with the
     * {@link Validation#RequisiteAIs} validation procedure.
     *
     * @return current status of the {@link Validation#RequisiteAIs} validation procedure
     * @deprecated Use {@link #getValidationEnabled(Validation)} instead
     */
    @Deprecated
    public boolean getValidateAIassociations() {
        return gs1encoderGetValidateAIassociationsJNI(ctx);
    }

    /**
     * Enable or disable the {@link Validation#RequisiteAIs} validation procedure.
     * <p>
     * <strong>Deprecated:</strong> Use {@link #setValidationEnabled(Validation, boolean)} instead.
     * <p>
     * This method is equivalent to calling {@link #setValidationEnabled(Validation, boolean)} with the
     * {@link Validation#RequisiteAIs} validation procedure.
     *
     * @param value {@code true} to enable the {@link Validation#RequisiteAIs} validation procedure; {@code false} to disable
     * @throws GS1EncoderParameterException if an error occurs
     * @deprecated Use {@link #setValidationEnabled(Validation, boolean)} instead
     */
    @Deprecated
    public void setValidateAIassociations(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetValidateAIassociationsJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the raw data that would be directly encoded within a GS1 barcode message.
     * <p>
     * This method reads the raw barcode data input buffer. The returned data represents
     * the actual message content that would be encoded in the barcode symbol, including
     * FNC1 characters represented as {@code "^"} for AI-based data.
     * <p>
     * The returned string remains valid until subsequent calls to methods that modify
     * the input data buffer such as {@link #setDataStr(String)}, {@link #setAIdataStr(String)},
     * or {@link #setScanData(String)}. If the content must persist beyond such calls,
     * it should be copied to a separate variable.
     *
     * @return the raw barcode data input buffer, or {@code null} if no data has been set
     * @see #setDataStr(String)
     * @see #getAIdataStr()
     * @see #setAIdataStr(String)
     * @see #setScanData(String)
     */
    public String getDataStr() {
        return gs1encoderGetDataStrJNI(ctx);
    }

    /**
     * Set the raw data that would be directly encoded within a GS1 barcode message.
     * <p>
     * A {@code "^"} character at the start of the input indicates that the data is in GS1
     * Application Identifier syntax. In this case, all subsequent instances of the
     * {@code "^"} character represent the FNC1 non-data characters that are used to
     * separate fields that are not specified as being pre-defined length from
     * subsequent fields.
     * <p>
     * Inputs beginning with {@code "^"} will be validated against certain data syntax
     * rules for GS1 AIs. If the input is invalid then this method will throw
     * a {@link GS1EncoderParameterException}.
     * In the case that the data is unacceptable due to invalid AI content then
     * a marked up version of the offending AI can be retrieved using {@link #getErrMarkup()}.
     * <p>
     * <strong>Note:</strong> It is strongly advised that GS1 data input is instead specified using
     * {@link #setAIdataStr(String)} which takes care of the AI encoding rules
     * automatically, including insertion of FNC1 characters where required. This
     * can be used for all symbologies that accept GS1 AI syntax data.
     * <p>
     * Inputs beginning with {@code "http://"} or {@code "https://"} will be parsed as a GS1
     * Digital Link URI during which the corresponding AI element string is
     * extracted and validated.
     * <p>
     * EAN/UPC, GS1 DataBar and GS1-128 support a Composite Component. The
     * Composite Component must be specified in AI syntax. It must be separated
     * from the primary linear components with a {@code "|"} character and begin with an
     * FNC1 in first position, for example:
     * <pre>
     * encoder.setDataStr("^0112345678901231|^10ABC123^11210630");
     * </pre>
     * <p>
     * The above specifies a linear component representing "(01)12345678901231"
     * together with a composite component representing "(10)ABC123(11)210630".
     * <p>
     * <strong>Note:</strong> For GS1 data it is simpler and less error prone to specify the input
     * in human-friendly GS1 AI syntax using {@link #setAIdataStr(String)}.
     *
     * @param value the raw barcode data to be set
     * @throws GS1EncoderParameterException if the provided data is invalid
     * @see #getDataStr()
     * @see #setAIdataStr(String)
     * @see #getErrMarkup()
     */
    public void setDataStr(String value) throws GS1EncoderParameterException {
        if (!gs1encoderSetDataStrJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the barcode data input buffer in human-friendly GS1 AI syntax.
     * <p>
     * Returns the barcode data in human-friendly GS1 AI syntax, or {@code null} if the input data
     * does not contain AI data.
     *
     * @return the barcode data in GS1 AI syntax, or {@code null} if not AI data
     */
    public String getAIdataStr() {
        return gs1encoderGetAIdataStrJNI(ctx);
    }

    /**
     * Set the barcode data input buffer using GS1 Application Identifier syntax.
     * <p>
     * The input is provided in human-friendly format <strong>without</strong> FNC1 characters
     * which are inserted automatically, for example:
     * <pre>(01)12345678901231(10)ABC123(11)210630</pre>
     * <p>
     * This syntax harmonises the format for the input accepted by all symbologies.
     * For example, the following input is acceptable for EAN-13, UPC-A, UPC-E, any
     * variant of the GS1 DataBar family, GS1 QR Code and GS1 DataMatrix:
     * <pre>(01)00031234000054</pre>
     * <p>
     * The input is immediately parsed and validated against certain rules for GS1 AIs, after
     * which the resulting encoding for valid inputs is available via {@link #getDataStr()}.
     * If the input is invalid then an exception will be thrown.
     * <p>
     * Any {@code "("} characters in AI element values must be escaped as {@code "\\("} to avoid
     * conflating them with the start of the next AI.
     * <p>
     * For symbologies that support a composite component (all except Data Matrix, QR Code,
     * and DotCode), the data for the linear and 2D components can be separated by a
     * {@code "|"} character, for example:
     * <pre>(01)12345678901231|(10)ABC123(11)210630</pre>
     *
     * @param value the barcode data in GS1 AI syntax
     * @throws GS1EncoderParameterException if the input is invalid
     */
    public void setAIdataStr(String value) throws GS1EncoderParameterException {
        if (!gs1encoderSetAIdataStrJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the expected scan data string that a reader should return.
     * <p>
     * Returns the string that should be returned by scanners when reading a
     * symbol that is an instance of the selected symbology and contains the same input data.
     * <p>
     * The output will be prefixed with the appropriate AIM symbology identifier.
     *
     * @return the expected scan data string, or {@code null} if invalid
     */
    public String getScanData() {
        return gs1encoderGetScanDataJNI(ctx);
    }

    /**
     * Process scan data received from a barcode reader.
     * <p>
     * Process normalised scan data received from a barcode reader with reporting of
     * AIM symbology identifiers enabled to extract the message data and perform syntax
     * checks in the case of GS1 Digital Link and AI data input.
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
     * A literal {@code "|"} character may be included in the scan data to indicate the
     * separation between the first and second messages that would be transmitted
     * by a reader that is configured to return the composite component when
     * reading EAN/UPC symbols.
     * <p>
     * Example scan data input: <pre>]C1011231231231233310ABC123{GS}99TESTING</pre>
     * where {GS} represents ASCII character 29.
     *
     * @param value the scan data string as read by a reader with AIM symbology identifiers enabled
     * @throws GS1EncoderScanDataException if the scan data is invalid
     */
    public void setScanData(String value) throws GS1EncoderScanDataException {
        if (!gs1encoderSetScanDataJNI(ctx, value))
            throw new GS1EncoderScanDataException(this.getErrMsg());
    }

    /**
     * Get the error markup generated when parsing AI data fails due to a linting failure.
     * <p>
     * When a setter function returns {@code false} (indicating an error), if that failure is due to
     * AI-based data being invalid, a marked up instance of the AI that failed will be generated.
     * <p>
     * Where it is meaningful to identify offending characters in the input data, these characters
     * will be surrounded by {@code "|"} characters. Otherwise the entire AI value will be surrounded by
     * {@code "|"} characters.
     *
     * @return marked up instance of the AI that failed validation, or empty string if no linting failure
     */
    public String getErrMarkup() {
        return gs1encoderGetErrMarkupJNI(ctx);
    }

    /**
     * Get a GS1 Digital Link URI that represents the AI-based input data.
     * <p>
     * This method converts AI-based input data into a GS1 Digital Link URI format.
     * <p>
     * Example: <pre>(01)12345678901231(10)ABC123(11)210630</pre> with stem
     * <code>https://id.example.com/stem</code> might produce:
     * <pre>https://id.example.com/stem/01/12345678901231?10=ABC123&amp;11=210630</pre>
     *
     * @param stem a URI "stem" used as a prefix for the URI. If null, the GS1 canonical stem (https://id.gs1.org/) will be used
     * @return a string representing the GS1 Digital Link URI for the input data
     * @throws GS1EncoderDigitalLinkException if invalid input was provided
     */
    public String getDLuri(String stem) throws GS1EncoderDigitalLinkException {
        String uri = gs1encoderGetDLuriJNI(ctx, stem);
        if (uri == null)
            throw new GS1EncoderDigitalLinkException(this.getErrMsg());
        return uri;
    }

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
     * @return an array of strings representing the HRI text
     */
    public String[] getHRI() {
        return gs1encoderGetHRIJNI(ctx);
    }

    /**
     * Get the non-numeric (ignored) query parameters from a GS1 Digital Link URI.
     * <p>
     * For example, if the input data buffer contains:
     * <pre>https://a/01/12312312312333/22/ABC?name=Donald%2dDuck&amp;99=ABC&amp;testing&amp;type=cartoon</pre>
     * <p>
     * Then this method returns: <code>name=Donald%2dDuck</code>, <code>testing</code>, <code>type=cartoon</code>
     * <p>
     * The returned strings are not URI decoded. The expected use for this method is to
     * present which sections of a given GS1 Digital Link URI have been ignored.
     *
     * @return an array of strings containing the non-numeric query parameters that were ignored
     */
    public String[] getDLignoredQueryParams() {
        return gs1encoderGetDLignoredQueryParamsJNI(ctx);
    }

}
