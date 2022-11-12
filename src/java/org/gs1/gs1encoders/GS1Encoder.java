package org.gs1.gs1encoders;

/**
 * Wrapper class for accessing the GS1 Syntax Engine native library from Java.
 *
 * This class implements a wrapper around the JNI interface to the GS1 Syntax
 * Engine native C library that presents its functionality in the form of a
 * typical Java object interface.
 *
 * This class is a very lightweight shim around the native library,
 * therefore the C# interface is described here in terms of the public
 * API functions of the native library that each method or property
 * getter/setter invokes.
 *
 * The API reference for the native C library is available here:
 *
 * https://gs1.github.io/gs1-syntax-engine/
 *
 * @author Copyright (c) 2022 GS1 AISBL.
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

public class GS1Encoder {

    /*
     *  Functions imported from the JNI interface
     *
     */
    private final static native String gs1encoderGetErrMsgJNI(long ctx);
    private final static native long gs1encoderInitJNI();
    private final static native void gs1encoderFreeJNI(long ctx);
    private final static native String gs1encoderGetVersionJNI();
    private final static native int gs1encoderGetSymJNI(long ctx);
    private final static native boolean gs1encoderSetSymJNI(long ctx, int value);
    private final static native boolean gs1encoderGetAddCheckDigitJNI(long ctx);
    private final static native boolean gs1encoderSetAddCheckDigitJNI(long ctx, boolean value);
    private final static native boolean gs1encoderGetIncludeDataTitlesInHRIJNI(long ctx);
    private final static native boolean gs1encoderSetIncludeDataTitlesInHRIJNI(long ctx, boolean value);
    private final static native boolean gs1encoderGetPermitUnknownAIsJNI(long ctx);
    private final static native boolean gs1encoderSetPermitUnknownAIsJNI(long ctx, boolean value);
    private final static native boolean gs1encoderGetValidateAIassociationsJNI(long ctx);
    private final static native boolean gs1encoderSetValidateAIassociationsJNI(long ctx, boolean value);
    private final static native String gs1encoderGetDataStrJNI(long ctx);
    private final static native boolean gs1encoderSetDataStrJNI(long ctx, String value);
    private final static native String gs1encoderGetAIdataStrJNI(long ctx);
    private final static native boolean gs1encoderSetAIdataStrJNI(long ctx, String value);
    private final static native String gs1encoderGetScanDataJNI(long ctx);
    private final static native boolean gs1encoderSetScanDataJNI(long ctx, String value);
    private final static native String gs1encoderGetErrMarkupJNI(long ctx);
    private final static native String gs1encoderGetDLuriJNI(long ctx, String stem);
    private final static native String[] gs1encoderGetHRIJNI(long ctx);
    private final static native String[] gs1encoderGetDLignoredQueryParamsJNI(long ctx);

    // Initialisation loads the gs1encoders JNI library interface
    static {
        System.loadLibrary("gs1encoders");
    }

    /**
     * List of symbology types, mirroring the corresponding list in the
     * C library.
     *
     * See the native library documentation for details:
     *
     *   - enum gs1_encoder_symbologies
     *
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
             * Value is the number of symbologies
             */
            NUMSYMS
    }

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
    private long ctx = 0;

    /*
     *  Methods to provide a wrapper around the functional interface imported from the native library
     *
     */

    // This Java wrapper library throws an excpetion containing the error message whenever
    // an error is returned by the native library. Therefore direct access to the native
    // error message is not necessary.
    private String getErrMsg() {
        return gs1encoderGetErrMsgJNI(ctx);
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
    public GS1Encoder() throws GS1EncoderGeneralException {
        ctx = gs1encoderInitJNI();
        if (ctx == 0)
            throw new GS1EncoderGeneralException("Failed to initalise the native library");
    }

    /**
     * Destructor that will release the resources allocated by the native library.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_free()
     *
     */
    public void free() {
        if (ctx != 0)
            gs1encoderFreeJNI(ctx);
        ctx = 0;
    }

    /**
     * Returns the version of the native library.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getVersion()
     *
     */
    public String getVersion() {
        return gs1encoderGetVersionJNI();
    }

    /**
     * Get the symbology type.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getVersion()
     *
     */
    public int getSym() {
        return gs1encoderGetSymJNI(ctx);
    }

    /**
     * Set the symbology type.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setVersion()
     *
     */
    public void setSym(int value) throws GS1EncoderParameterException {
        if (!gs1encoderSetSymJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getAddCheckDigit()
     *
     */
    public boolean getAddCheckDigit() {
        return gs1encoderGetAddCheckDigitJNI(ctx);
    }

    /**
     * Set the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setAddCheckDigit()
     *
     */
    public void setAddCheckDigit(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetAddCheckDigitJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the "include data titles in HRI" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getIncludeDataTitlesInHRI()
     *
     */
    public boolean getIncludeDataTitlesInHRI() {
        return gs1encoderGetIncludeDataTitlesInHRIJNI(ctx);
    }

    /**
     * Set the "include data titles in HRI" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setIncludeDataTitlesInHRI()
     *
     */
    public void setIncludeDataTitlesInHRI(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetIncludeDataTitlesInHRIJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the "permit unknown AIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getPermitUnknownAIs()
     *
     */
    public boolean getPermitUnknownAIs() {
        return gs1encoderGetPermitUnknownAIsJNI(ctx);
    }

    /**
     * Set the "permit unknown AIs" mode.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setPermitUnknownAIs()
     *
     */
    public void setPermitUnknownAIs(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetPermitUnknownAIsJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the "validate AI associations" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getValidateAIassociations()
     *
     */
    public boolean getValidateAIassociations() {
        return gs1encoderGetValidateAIassociationsJNI(ctx);
    }

    /**
     * Set the "validate AI associations" flag.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setValidateAIassociations()
     *
     */
    public void setValidateAIassociations(boolean value) throws GS1EncoderParameterException {
        if (!gs1encoderSetValidateAIassociationsJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the raw barcode data input buffer.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getDataStr()
     *
     */
    public String getDataStr() {
        return gs1encoderGetDataStrJNI(ctx);
    }

    /**
     * Set the raw barcode data input buffer.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setDataStr()
     *
     */
    public void setDataStr(String value) throws GS1EncoderParameterException {
        if (!gs1encoderSetDataStrJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the barcode data input buffer using GS1 AI syntax.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getAIdataStr()
     *
     */
    public String getAIdataStr() {
        return gs1encoderGetAIdataStrJNI(ctx);
    }

    /**
     * Set the barcode data input buffer using GS1 AI syntax.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setAIdataStr()
     *
     */
    public void setAIdataStr(String value) throws GS1EncoderParameterException {
        if (!gs1encoderSetAIdataStrJNI(ctx, value))
            throw new GS1EncoderParameterException(this.getErrMsg());
    }

    /**
     * Get the barcode data input buffer using barcode scan data format.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getScanData()
     *
     */
    public String getScanData() {
        return gs1encoderGetScanDataJNI(ctx);
    }

    /**
     * Set the barcode data input buffer using barcode scan data format.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_setScanData()
     *
     */
    public void setScanData(String value) throws GS1EncoderScanDataException {
        if (!gs1encoderSetScanDataJNI(ctx, value))
            throw new GS1EncoderScanDataException(this.getErrMsg());
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
    public String getErrMarkup() {
        return gs1encoderGetErrMarkupJNI(ctx);
    }

    /**
     * Get a Digital Link URI that represents the AI-based input data.
     *
     * See the native library documentation for details:
     *
     *   - gs1_encoder_getDLuri()
     *
     */
    public String getDLuri(String stem) throws GS1EncoderDigitalLinkException {
        String uri = gs1encoderGetDLuriJNI(ctx, stem);
        if (uri.isEmpty())
            throw new GS1EncoderDigitalLinkException(this.getErrMsg());
        return uri;
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
    public String[] getHRI() {
        return gs1encoderGetHRIJNI(ctx);
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
    public String[] getDLignoredQueryParams() {
        return gs1encoderGetDLignoredQueryParamsJNI(ctx);
    }

}