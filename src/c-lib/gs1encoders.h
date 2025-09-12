/**
 * GS1 Barcode Syntax Engine
 *
 * @file gs1encoders.h
 * @author GS1 AISBL
 *
 * \copyright Copyright (c) 2000-2025 GS1 AISBL.
 *
 * @licenseblock{License}
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
 * @endlicenseblock
 *
 *
 * Overview
 * --------
 *
 * The GS1 Barcode Syntax Engine provides routines that support the processing of GS1
 * syntax data, including Application Identifier element strings and GS1
 * Digital Link URIs, whether these are provided in raw or human-friendly
 * format or as normalised scan data received from barcode readers.
 *
 * The implementations are intended for use with GS1 standards and applications
 * and do not contain additional features that might be required for more
 * general use.
 *
 * Within the GS1 Application Identifier system, structured data is represented
 * in different formats depending upon the context.
 *
 * The data formats supported by this library are:
 *
 *   * **Bracketed AI element strings**: Human-friendly rendition of AI data as a single string using numerical AIs.
 *   * **Unbracketed AI element strings**: Rendition of AI data that corresponds most directly to encoded barcode data.
 *   * **GS1 Digital Link URIs**
 *   * **Scan data**: The expected result of scanning a symbol with a barcode reader that has AIM symbologies identifiers enabled.
 *   * **Human Readable Interpretation (HRI)**: Human-friendly redition of the AI data contained within a symbol. This may also include Data Titles to present the AI data in the form of "mixed HRI/non-HRI text". (Output only.)
 *
 * This following diagram shows how the library can be used for processing and
 * transformation of GS1 data, indicating which formats are accepted as input,
 * how barcode message data is generated and AI data extracted from the
 * provided input data, and how the given data can be output in various
 * formats.
 *
 * | Data transformation: Inputs, outputs and buffers |
 * | :----------------------------------------------- |
 * | \image html input_output_buffers.svg             |
 *
 * The above diagram highlights that conceptually the library contains two
 * internal "data buffers":
 *
 *   * **Barcode message buffer:** This is populated with the raw message
 *     that would be borne by a GS1 barcode symbol that represents the input
 *     data, e.g. unbracketed AI syntax with FNC1 in first for regular AI
 *     element strings; plain string for a plain data or a GS1 Digital Link
 *     URI.
 *
 *   * **Extracted AI buffer:** This contains the in-order AI data that was
 *     extracted from the input data.
 *
 * The main operations of the library involve reading and updating the state of
 * these buffers.
 *
 * Example Uses
 * ------------
 *
 * The following are examples of how to use the library.
 *
 * \note
 * Using the library always begins by initialising the library with
 * gs1_encoder_init() and finishes by releasing the library with
 * gs1_encoder_free().
 *
 * \note
 * Unless otherwise specified, the getter functions return pointers to
 * per-instance storage managed by this library and therefore must not be freed
 * by the user. If their content must persist following a subsequent call to
 * the same instance of the library then they must be copied to a user-managed
 * buffer.
 *
 * \note
 * Most of the setter and action functions of this library return a boolean
 * indicating whether the function was successful and write an error message
 * that can be accessed with gs1_encoder_getErrMsg() in the event of failure.
 * Production code should check the output of the functions and where relevant
 * do something appropriate which might include rendering the error message to
 * the user. Error message string are provided in the English language in a
 * single file that can be replaced at compile time.
 *
 * Refer to the example console application (`gs1encoders-app.c`) for a
 * comprehensive example of how to use this library.
 *
 *
 * ### GS1 AI data validation and extraction (including GS1 Digital Link)
 *
 * The following code processes AI data input, validates it (reporting any
 * failures) and displays the extracted AIs if the validation succeeds.
 *
 * \code
 * gs1_encoder *ctx = gs1_encoder_init(NULL);              // Create a new instance of the library
 *
 * // gs1_encoder_permitUnknownAIs(ctx, true);             // Uncomment only if it is necessary to handle AIs
 *                                                         // that are not known to the library
 *
 * // Input provided as a bracketed AI element string
 * //
 * bool ret = gs1_encoder_setAIdataStr(ctx, "(01)12312312312333(10)ABC123(99)TEST");
 *
 * // Alternatively, the input may be given in the following formats:
 * //
 * // bool ret = gs1_encoder_setDataStr(ctx,               // Unbracketed element string, "^" = FNC1
 * //                "^011231231231233310ABC123^99TEST");
 * //
 * // bool ret = gs1_encoder_setDataStr(ctx,               // GS1 Digital Link URI
 * //                "https://example.com/01/12312312312333/10/ABC123/99/TEST");
 * //
 * // bool ret = gs1_encoder_setScanData(ctx,              // Barcode scan data, containing a "GS" (ASCII 0x1D) separator
 * //                "]Q1011231231231233310ABC123" "\x1D" "99TEST");
 *
 * if (!ret) {
 *     printf("ERROR: %s\n", gs1_encoder_getErrMsg(ctx));  // Display a descriptive error message
 *     char *errMarkup = gs1_encoder_getErrMarkup(ctx);
 *     if (*errMarkup != '\0')                             // Display the invalid AI in the case of a Linting failure
 *         printf("Bad AI data: %s\n", errMarkup);
 *     abort();                                            // Finally, handle the error in an application-specific way
 * }
 *
 * char **hri;
 * int numHRI = gs1_encoder_getHRI(ctx, &hri);             // Display the extracted AI data as HRI text
 * for (int i = 0; i < numHRI; i++) {
 *     printf("%s\n", hri[i]);
 * }
 *
 * gs1_encoder_free(ctx);                                  // Release the instance of the library
 * \endcode
 *
 *
 * ### Converting an AI element string to barcode message data
 *
 * In this example we process a bracketed AI element string to convert it into
 * barcode message data, suitable for carrying in a GS1 barcode symbol.
 *
 * \code
 * gs1_encoder *ctx = gs1_encoder_init(NULL);
 *
 * bool ret = gs1_encoder_setAIdataStr(ctx,        // Accept a bracketed AI element string
 *     "(01)12312312312333(10)ABC123(99)TEST");
 *
 * if (!ret) {
 *     // Handle error and return
 * }
 *
 * printf("%s\n", gs1_encoder_getDataStr(ctx));    // Render the barcode message buffer
 *
 * gs1_encoder_free(ctx);
 * \endcode
 *
 * \note
 * The barcode message data read and emitted by this library uses a harmonised
 * format that does not concern itself with internal encoding quirks of various
 * symbologies. In the harmonised barcode message data:
 * \note
 *   - A leading `^` always indicates GS1 Application Identifier syntax data,
 *   i.e. a notional FNC1 in first character position. (This is even true for
 *   DotCode in whose *internal encoding* the literal FNC1 non-data character
 *   may have an inverted meaning for certain messages depending upon their
 *   regular data content.)
 *   - A `^` at any other position represents a notional FNC1 non-data
 *   Application Identifier separator character. (This is even the case for QR
 *   Code in whose *internal encoding* a `%` character or `{GS}` character
 *   takes on the AI separator role typically assigned to the FNC1 non-data
 *   character, depending upon the effectuve encodation mode.)
 * \note
 * Additionally, barcode image encoder libraries have differing conventions for
 * how to input FNC1 characters, extending to whether it is necessary to be
 * explicit about the notional FNC1 character in the first position when
 * specifying a GS1 Application Identifier syntax symbol.
 * \note
 * Consequently, the barcode message data emitted by this library may need to
 * be post-processed to align to the specific requirements of whatever symbol
 * generation library is in use.
 *
 *
 * ### Barcode scan data processing
 *
 * In this example we process scan data from a barcode reader to extract the AI
 * data.
 *
 * \code
 * gs1_encoder *ctx = gs1_encoder_init(NULL);
 *
 * // Disable validation of mandatory association between AIs if the symbol may
 * // be one of multiple on a label
 * setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, false);
 *
 * bool ret = gs1_encoder_setScanData(ctx,
 *                "]Q1011231231231233310ABC123" "\x1D" "99TEST");
 *
 * if (!ret) {
 *     // Handle error and return
 * }
 *
 * char **hri;
 * int numHRI = gs1_encoder_getHRI(ctx, &hri);
 * for (int i = 0; i < numHRI; i++) {
 *     printf("%s\n", hri[i]);
 * }
 *
 * // If it is necessary to know the "symbology" that was scanned then this can
 * // be read using gs1_encoder_getSym(), however note the caveats given in the
 * // description of gs1_encoder_setScanData()
 *
 * gs1_encoder_free(ctx);
 * \endcode
 *
 * \note
 * It is required that AIM Symbology Identifiers are enabled on the barcode
 * reader.
 *
 * \note
 * It is assumed the scanned barcode message "survives the channel" intact,
 * i.e. that no character substitutions have been made by the reader, in
 * particular that any embedded FNC1 separator characters are correctly
 * represented by GS characters (ASCII 29). If this is not the case then the
 * scanned data should be pre-processed to meet this requirement.
 *
 *
 * Compile-Time Configuration Macros
 * ---------------------------------
 *
 * The following macros may be defined at compile time to enable or disable
 * optional features:
 *
 * `GS1_ENCODERS_ERR_LANG=<LANG>`
 * :  Used to specify alternative translation strings for error messages.
 *    Create a new header file named `tr_<LANG>.h` based on the existing
 *    `tr_EN.h` file.
 *
 * `EXCLUDE_EMBEDDED_AI_TABLE`
 * :  Excludes the embedding a table of AI properties in order to reduce the
 *    size of the library. The table of AI definitions must be populated by
 *    parsing from the GS1 Syntax Dictionary instead, without falling back to
 *    the embedded instance.
 *
 * `EXCLUDE_SYNTAX_DICTIONARY_LOADER`
 * :  Excludes functions for populating the table of AIs by parsing the GS1
 *    Syntax Dictionary. The AI definitions must be embedded for the library to
 *    function, i.e. this option is mutually exclusive of
 *    `EXCLUDE_EMBEDDED_AI_TABLE`.
 *
 * `GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H=<CUSTOM_HEADER.h>`
 * :  Points to a file that declares alternative heap management routines via
 *    the `GS1_ENCODERS_CUSTOM_MALLOC`, `GS1_ENCODERS_CUSTOM_CALLOC`,
 *    `GS1_ENCODERS_CUSTOM_REALLOC` and `GS1_ENCODERS_CUSTOM_FREE` macros. See
 *    below for implementation details.
 *
 * ### Custom heap management routines example
 *
 * Define `GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H=my_alloc.h`.
 *
 * Example declarations in `my_alloc.h`:
 *
 * \code
 * #define GS1_ENCODERS_CUSTOM_MALLOC(sz) my_malloc(sz)
 * #define GS1_ENCODERS_CUSTOM_CALLOC(nm, sz) my_calloc(nm, sz)
 * #define GS1_ENCODERS_CUSTOM_REALLOC(p, sz) my_realloc(p, sz)
 * #define GS1_ENCODERS_CUSTOM_FREE(p) my_free(p)
 *
 * void* my_malloc(size_t s);
 * void* my_calloc(size_t nm, size_t sz);
 * void* my_realloc(void *p, size_t sz);
 * void my_free(void *p);
 * \endcode
 *
 * Example implementation:
 *
 * \code
 * ...
 *
 * void* my_malloc(size_t s) {
 *         void* p = malloc(s);
 *         printf("*** MALLOC %zu => %p\n", s, p);
 *         return p;
 * }
 *
 * void* my_calloc(size_t nm, size_t sz) {
 *         void* p = calloc(nm, sz);
 *         printf("*** CALLOC %zu %zu => %p\n", nm, sz, p);
 *         return p;
 * }
 *
 * void* my_realloc(void *p, size_t sz) {
 *         void* q = realloc(p, sz);
 *         printf("*** REALLOC %p %zu => %p\n", p, sz, q);
 *         return q;
 * }
 *
 * void my_free(void *p) {
 *         printf("*** FREE %p\n", p);
 *         free(p);
 * }
 *
 * ...
 * \endcode
 */


#ifndef GS1_ENCODERS_H
#define GS1_ENCODERS_H

/// \cond
#include <stdbool.h>
#include <stddef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
/// \endcond


// Decorator for public API functions that we export
#ifdef __EMSCRIPTEN__
#  define GS1_ENCODERS_API EMSCRIPTEN_KEEPALIVE
#elif defined(_WIN32)
#  define GS1_ENCODERS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#  define GS1_ENCODERS_API __attribute__((visibility ("default")))
#else
#  define GS1_ENCODERS_API
#endif


#if defined(__GNUC__) || defined(__clang__)
#  define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#  define DEPRECATED __declspec(deprecated)
#else
#  define DEPRECATED
#endif


#ifdef __cplusplus
extern "C" {
#endif


/// Recognised GS1 barcode formats ("symbologies") for processing scan data.
enum gs1_encoder_symbologies {
	gs1_encoder_sNONE = -1,			///< None defined
	gs1_encoder_sDataBarOmni,		///< GS1 DataBar Omnidirectional
	gs1_encoder_sDataBarTruncated,		///< GS1 DataBar Truncated
	gs1_encoder_sDataBarStacked,		///< GS1 DataBar Stacked
	gs1_encoder_sDataBarStackedOmni,	///< GS1 DataBar Stacked Omnidirectional
	gs1_encoder_sDataBarLimited,		///< GS1 DataBar Limited
	gs1_encoder_sDataBarExpanded,		///< GS1 DataBar Expanded (Stacked)
	gs1_encoder_sUPCA,			///< UPC-A
	gs1_encoder_sUPCE,			///< UPC-E
	gs1_encoder_sEAN13,			///< EAN-13
	gs1_encoder_sEAN8,			///< EAN-8
	gs1_encoder_sGS1_128_CCA,		///< GS1-128 with CC-A or CC-B
	gs1_encoder_sGS1_128_CCC,		///< GS1-128 with CC-C
	gs1_encoder_sQR,			///< (GS1) QR Code
	gs1_encoder_sDM,			///< (GS1) Data Matrix
	gs1_encoder_sDotCode,			///< (GS1) DotCode
	gs1_encoder_sNUMSYMS,
};

/**
 * @brief Equivalent to the `enum gs1_encoder_symbologies` type.
 *
 */
typedef enum gs1_encoder_symbologies gs1_encoder_symbologies_t;


/// Optional AI validation procedures that may be applied to detect invalid inputs when AI data is provided using gs1_encoder_setAIdataStr(), gs1_encoder_setDataStr() or gs1_encoder_setScanData().
/// @note Only AI validation procedures whose "enabled" status can be updated (i.e. not "locked") are described.
enum gs1_encoder_validations {
	// Exported as API. Not to be re-ordered.
	gs1_encoder_vMUTEX_AIS = 0,
	gs1_encoder_vREQUISITE_AIS,		///< **Default: Enabled**. Validates that the input satisfies the mandatory associations for each AI.
	gs1_encoder_vREPEATED_AIS,
	gs1_encoder_vDIGSIG_SERIAL_KEY,
	gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR,	///< **Default: Enabled**. Unknown AIs will not be accepted as GS1 DL URI data attributes.
	gs1_encoder_vNUMVALIDATIONS,
};


/**
 * @brief Equivalent to the `enum gs1_encoder_validations` type.
 *
 */
typedef enum gs1_encoder_validations gs1_encoder_validations_t;


/**
 * @brief A gs1_encoder context.
 *
 * This is an opaque struct that represents an instance of the library.
 *
 * This context maintains all state required for an instance. Any number of
 * instances of the library can be created, each operating separately and
 * equivalently to the others.
 *
 * This library does not introduce any global variables. All runtime state
 * is maintained in instances of the ::gs1_encoder struct and this state should
 * only be modified using the public API functions provided by this library,
 * decorated with GS1_ENCODERS_API.
 *
 * A context is created by calling gs1_encoder_init() and destroyed by calling
 * gs1_encoder_free(), releasing all of the storage allocated by the library
 * for that instance.
 *
 * \note
 * This struct is deliberately opaque and it's layout should be assumed to vary
 * between releases of the library and even between builds created with
 * different options.
 *
 * \note
 * The library is thread-safe provided that each thread operates on its own
 * instance of the library.
 *
 */
typedef struct gs1_encoder gs1_encoder;


/**
 * @brief Get the version string of the library.
 *
 * This is typically the build date.
 *
 * The return data does not need to be free()ed.
 *
 * @return pointer to a string containing the version of the library
 */
GS1_ENCODERS_API char* gs1_encoder_getVersion(void);


/**
 * @brief Find the memory storage requirements for an instance of ::gs1_encoder.
 *
 * For embedded systems it may be desirable to provide a pre-allocated buffer
 * to a new context for storage purposes, rather than to have the instance
 * malloc() it's own storage. This may avoid problems such as heap
 * fragmentation on systems with a poor memory allocator or a restricted
 * working set size.
 *
 * This function returns the minimum size required for such a buffer.
 *
 * Example of a user of the library allocating its own heap storage:
 *
 * \code{.c}
 * gs1_encoder *ctx;
 * size_t mem = gs1_encoder_instanceSize();
 * void *heap = malloc(mem);
 * assert(heap);
 * ctx = gs1_encoder_init(heap);
 * ...
 * gs1_encoder_free(ctx);
 * free(heap);
 * \endcode
 *
 * Example of a user of the library using static storage allocated at compile
 * time:
 *
 * \code{.c}
 * static uint8_t prealloc[65536];  // Ensure that this is big enough
 * ...
 * void myFunc(void) {
 * 	gs1_encoder *ctx;
 * 	size_t mem = gs1_encoder_instanceSize();
 * 	assert(sizeof(prealloc) <= mem);
 * 	ctx = gs1_encoder_init((void *)prealloc);
 * 	...
 * }
 * \endcode
 *
 * @return memory required to hold a context instance
 */
GS1_ENCODERS_API size_t gs1_encoder_instanceSize(void);


/**
 * @brief Get the maximum size of the input data buffer for barcode message content.
 *
 * This is an implementation limit that may be lowered for systems with limited
 * memory by rebuilding the library.
 *
 * \note
 * In practise each barcode symbology has its own data capacity that may be
 * somewhat less than this maximum size.
 *
 * @see gs1_encoder_setDataStr()
 *
 * @return maximum number bytes that can be supplied for encoding
 */
GS1_ENCODERS_API int gs1_encoder_getMaxDataStrLength(void);


/**
 * @brief Initialise a new ::gs1_encoder context.
 *
 * If it expected that most users of the library will pass NULL to this
 * function which causes the library will allocate its own storage.
 *
 * If a pointer to a storage buffer is provided then this will be used instead,
 * however this buffer must be sufficient for the needs of the instance as
 * returned by gs1_encoder_instanceSize() and this buffer should not be reused
 * or freed until gs1_encoder_free() is called.
 *
 * @see gs1_encoder_instanceSize()
 *
 * @param [in,out] mem buffer to use for storage, or NULL for automatic allocation
 * @return ::gs1_encoder context on success, else NULL.
 */
GS1_ENCODERS_API gs1_encoder* gs1_encoder_init(void *mem);


/**
 * @brief Read an error message generated by the library.
 *
 * When any of the setter functions of this library return false (indicating an
 * error), a human-friendly error message is generated which can be read using
 * this function.
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after any subsequent library function
 * calls.
 *
 * ### Data validation checks
 *
 * The library may return a single error message for each data operation. When
 * setting message data with gs1_encoder_setDataStr(),
 * gs1_encoder_setAIdataStr() or gs1_encoder_setScanData() it may be useful to
 * understand the order in which data validation checks are performed.
 *
 * In general, the data input is validated by applying checks in the following
 * outline sequence:
 *
 *   * For bracketed AI element strings:
 *     * Ensure that the bracket structure is correct allowing AI extraction.
 *     * For each extracted AI, validate that the data corresponds to a known
 *       AI, unless disabled using gs1_encoder_setPermitUnknownAIs(), and that
 *       the overall length is within limits.
 *   * For unbracked AI element strings:
 *     * Validate FNC1 in first position and that the data is not empty.
 *     * For each extracted AI prefix, validate that the data corresponds to
 *       known AI prefixes and that AIs are correctly terminated by FNC1, where
 *       this is required.
 *   * For GS1 Digital Link URIs:
 *     * Ensure that the data does not contain illegal URI characters
 *     * Ensure that the scheme, domain and stem are present.
 *     * Ensure that the path info structure is correct allowing for AI
 *       extraction.
 *     * For each extracted AI, validate that the data corresponds to a known
 *       AI, unless disabled using gs1_encoder_setPermitUnknownAIs(), and that
 *       the overall length is within limits.
 *     * Ensure that the AIs extracted from the path info form a valid primary
 *       key to key-qualifier association.
 *     * Ensure that the query parameter structure is correct allowing for AI extraction.
 *     * Ensure that the AIs extracted from the query parameters are valid GS1 DL URI data attributes.
 *     * Ensure that there are no duplicate AIs within any part of the URI structure.
 *   * Then, for all AI-based data (bracketed, unbracketed and GS1 Digital Link URIs):
 *     * For each component of each AI, as defined by the GS1 Barcode Syntax Dictionary:
 *       * Validate that its length is within the specified limits.
 *       * Validate that it conforms to the specified character set.
 *       * Apply each of the specified GS1 Barcode Syntax Tests ("linters") in turn.
 *     * Validate the overall AI data and associations for each validation process, except where they are disabled (either by default or manually via gs1_encoder_setValidationEnabled()), for example:
 *       * Ensure that repeated AIs have the same value.
 *       * Ensure that mutually-exclusive AIs are not present.
 *       * Ensure that all requisite AIs are accounted for.
 *       * Ensure that serialisation qualifier AIs are present with Digital Signature.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return pointer to error message string
 */
GS1_ENCODERS_API char* gs1_encoder_getErrMsg(gs1_encoder *ctx);


/**
 * @brief Read the error markup generated when parsing AI data fails due to a
 * linting failure.
 *
 * When any of the setter functions of this library return false (indicating an
 * error), then if that failure is due to the AI-based data being invalid a
 * marked up instance of the AI that failed will be generated which can be read
 * using this function.
 *
 * Where it is meaningful to identify offending characters in the input data,
 * these characters will be surrounded by `|` characters. Otherwise the
 * entire AI value will be surrounded by `|` characters.
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after any subsequent library function
 * calls.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return pointer to the markup for the AI linting failure
 */
GS1_ENCODERS_API char* gs1_encoder_getErrMarkup(gs1_encoder *ctx);


/**
 * @brief Get the current symbology type.
 *
 * \note
 * This might be set manually via gs1_encoder_setSym() or automatically when
 * processing scan data with gs1_encoder_setScanData.
 *
 * @see ::gs1_encoder_symbologies
 * @see gs1_encoder_setSym()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return symbology type, one of ::gs1_encoder_symbologies
 */
GS1_ENCODERS_API gs1_encoder_symbologies_t gs1_encoder_getSym(gs1_encoder *ctx);


/**
 * @brief Set the symbology type.
 *
 * This allows the symbology to be specified as any one of the described
 * ::gs1_encoder_symbologies
 *
 * @see ::gs1_encoder_symbologies
 * @see gs1_encoder_getSym()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] sym symbology type
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setSym(gs1_encoder *ctx, gs1_encoder_symbologies_t sym);


/**
 * @brief Get the current status of the "add check digit" mode.
 *
 * @see gs1_encoder_setAddCheckDigit()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return current status of the add check digit mode
 */
GS1_ENCODERS_API bool gs1_encoder_getAddCheckDigit(gs1_encoder *ctx);


/**
 * @brief Enable or disable "add check digit" mode for EAN/UPC and GS1 DataBar
 * symbols.
 *
 *   * If false (default), then the data string must contain a valid check digit.
 *   * If true, then the data string must not contain a check digit as one will
 *     be generated automatically.
 *
 * \note
 * This option is only valid for symbologies that accept fixed-length data,
 * specifically EAN/UPC and GS1 DataBar except Expanded (Stacked).
 *
 * @see gs1_encoder_getAddCheckDigit()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] addCheckDigit enabled if true; disabled if false
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setAddCheckDigit(gs1_encoder *ctx, bool addCheckDigit);


/**
 * @brief Get the current status of the "permit unknown AIs" mode.
 *
 * @see gs1_encoder_setPermitUnknownAIs()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return current status of the permit unknown AIs mode
 */
GS1_ENCODERS_API bool gs1_encoder_getPermitUnknownAIs(gs1_encoder *ctx);


/**
 * @brief Enable or disable "permit unknown AIs" mode for parsing of bracketed
 * AI element strings and GS1 Digital Link URIs.
 *
 *   * If false (default), then all AIs represented by the input data must be
 *     known.
 *   * If true, then unknown AIs (those not in this library's static AI table)
 *     will not be accepted.
 *
 * \note
 * The option only applies to parsed input data, specifically bracketed AI data
 * supplied with gs1_encoder_setAIdataStr() and GS1 Digital Link URIs supplied
 * with gs1_encoder_setDataStr(). Unbracketed AI element strings containing
 * unknown AIs cannot be parsed because it is not possible to differentiate the
 * AI from its data value when the length of the AI is uncertain.
 *
 * @see gs1_encoder_getPermitUnknownAIs()
 * @see gs1_encoder_setAIdataStr()
 * @see gs1_encoder_setDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] permitUnknownAIs enabled if true; disabled if false
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setPermitUnknownAIs(gs1_encoder *ctx, bool permitUnknownAIs);


/**
 * @brief Get the current status of the "permit zero-suppressed GTIN in GS1 DL URIs" mode.
 *
 * @see gs1_encoder_setPermitZeroSuppressedGTINinDLuris()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return current status of the permit zero-suppressed GTINs in GS1 DL URIs mode
 */
GS1_ENCODERS_API bool gs1_encoder_getPermitZeroSuppressedGTINinDLuris(gs1_encoder *ctx);


/**
 * @brief Enable or disable "permit zero-suppressed GTIN in GS1 DL URIs" mode for
 * parsing of GS1 Digital Link URIs.
 *
 *   * If false (default), then the value of a path component for AI (01) must
 *     be provided as a full GTIN-14.
 *   * If true, then the value of a path component for AI (01) may contain the
 *     GTIN-14 with zeros suppressed, in the format of a GTIN-13, GTIN-12 or
 *     GTIN-8.
 *
 * \note
 * The option only applies to parsed input data, specifically GS1 Digital Link
 * URIs supplied with gs1_encoder_setDataStr(). Since zero-suppressed GTINs are
 * deprecated, this option should only be enabled when it is necessary to
 * accept legacy GS1 Digital Link URIs having zero-suppressed GTIN-14.
 *
 * @see gs1_encoder_getPermitZeroSuppressedGTINinDLuris()
 * @see gs1_encoder_setDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] permitZeroSuppressedGTINinDLuris enabled if true; disabled if false
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setPermitZeroSuppressedGTINinDLuris(gs1_encoder *ctx, bool permitZeroSuppressedGTINinDLuris);


/**
 * @brief Get the current status of the "include data titles in HRI" flag.
 *
 * @see gs1_encoder_setIncludeDataTitlesInHRI()
 * @see gs1_encoder_getHRI()
 * @see gs1_encoder_copyHRI()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return current status of the include data titles in HRI flag
 */
GS1_ENCODERS_API bool gs1_encoder_getIncludeDataTitlesInHRI(gs1_encoder *ctx);


/**
 * @brief Enable or disable "include data titles in HRI" flag.
 *
 *   * If false (default), then the HRI data string produced by gs1_encoder_getHRI() and gs1_encoder_copyHRI() will not include data titles
 *   * If true, then the data string produced by gs1_encoder_getHRI() and gs1_encoder_copyHRI() will include data titles
 *
 * @see gs1_encoder_getIncludeDataTitlesInHRI()
 * @see gs1_encoder_getHRI()
 * @see gs1_encoder_copyHRI()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] includeDataTitles enabled if true; disabled if false
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setIncludeDataTitlesInHRI(gs1_encoder *ctx, bool includeDataTitles);


/**
 * @brief Get the current enabled status of the provided AI validation procedure
 *
 * @see gs1_encoder_setValidationEnabled()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] validation a validation procedure of type ::gs1_encoder_validations to check the status of
 * @return true if the AI validation procedure is currently enabled, false otherwise including when the procedure is unknown
 */
GS1_ENCODERS_API bool gs1_encoder_getValidationEnabled(gs1_encoder *ctx, gs1_encoder_validations_t validation);


/**
 * @brief Enable or disable the given AI validation procedure of type
 * ::gs1_encoder_validations, that determines whether certain checks are
 * enforced when data is provided using gs1_encoder_setAIdataStr(),
 * gs1_encoder_setDataStr() or gs1_encoder_setScanData().
 *
 *   * If enabled is true (default), then the corresponding validation will be enforced.
 *   * If enabled is false, then the corresponding validation will not be enforced.
 *
 * \note
 * The option only applies to AI input data, specifically AI data supplied with
 * gs1_encoder_setAIdataStr() or gs1_encoder_setDataStr(), or GS1 Digital Link
 * URIs supplied with gs1_encoder_setDataStr(). Under certain circumstances it
 * may be necessary to disable certain AI validation procedures in order to
 * satisfy checks when the provided AI data represents only part if the overall
 * AI data on a label.
 *
 * Nevertheless, the full AI data should be validated by concatinating it and
 * verifying it with this library with all relivant AI validation procedures
 * enabled.
 *
 * @see gs1_encoder_getValidationEnabled()
 * @see gs1_encoder_setAIdataStr()
 * @see gs1_encoder_setDataStr()
 * @see gs1_encoder_setScanData()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] validation a validation procedure of type ::gs1_encoder_validations to attempt to set the enabled status of
 * @param [in] enabled validation is enabled if true; disabled if false
 * @return true on success, otherwise false (for example when an attempt is made to modify a "locked" validation) and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API bool gs1_encoder_setValidationEnabled(gs1_encoder *ctx, gs1_encoder_validations_t validation, bool enabled);


/**
 * @brief Provided for backwards compatibility to get the current enabled
 * status of the ::gs1_encoder_vREQUISITE_AIS validation procedure.
 *
 * @deprecated This is equivalent to `gs1_encoder_getValidationEnabled(ctx,
 * gs1_encoder_vREQUISITE_AIS)`, which should be called instead.
 *
 * @see gs1_encoder_getValidationEnabled()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return current status of the ::gs1_encoder_vREQUISITE_AIS validation procedure
 */
GS1_ENCODERS_API DEPRECATED bool gs1_encoder_getValidateAIassociations(gs1_encoder *ctx);


/**
 * @brief Provided for backwards compatibility to enable or disable the
 * ::gs1_encoder_vREQUISITE_AIS validation procedure, that determines whether
 * mandatory AI pairings are enforced when data is provided using
 * gs1_encoder_setAIdataStr(), gs1_encoder_setDataStr() or
 * gs1_encoder_setScanData().
 *
 * @deprecated This is equivalent to `gs1_encoder_setValidationEnabled(ctx,
 * gs1_encoder_vREQUISITE_AIS, enabled)`, which should be called instead.
 *
 * @see gs1_encoder_setValidationEnabled()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] validateAIassociations enable the ::gs1_encoder_vREQUISITE_AIS validation procedure if true; otherwise disable
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg()
 */
GS1_ENCODERS_API DEPRECATED bool gs1_encoder_setValidateAIassociations(gs1_encoder *ctx, bool validateAIassociations);


/**
 * @brief Reads the raw barcode data input buffer.
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to library
 * function that modify the input data buffer such as gs1_encoder_setDataStr(),
 * gs1_encoder_setAIdataStr() and gs1_encoder_setScanData().
 *
 * @see gs1_encoder_getDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return a pointer to the data input buffer
 */
GS1_ENCODERS_API char* gs1_encoder_getDataStr(gs1_encoder *ctx);


/**
 * @brief Sets the raw data that would be directly encoded within a GS1 barcode
 * message.
 *
 * A "^" character at the start of the input indicates that the data is in GS1
 * Application Identifier syntax. In this case, all subsequent instances of the
 * "^" character represent the FNC1 non-data characters that are used to
 * separate fields that are not specified as being pre-defined length from
 * subsequent fields.
 *
 * Inputs beginning with "^" will be validated against certain data syntax
 * rules for GS1 AIs. If the input is invalid then this function will return
 * false and an error message will be set that can be read using
 * gs1_encoder_getErrMsg(). In the case that the data is unacceptable due to
 * invalid AI content then a marked up version of the offending AI can be
 * retrieved using gs1_encoder_getErrMarkup().
 *
 * It is strongly advised that GS1 data input is instead specified using
 * gs1_encoder_setAIdataStr() which takes care of the AI encoding rules
 * automatically, including insertion of FNC1 characters where required. This
 * can be used for all symbologies that accept GS1 AI syntax data.
 *
 * Inputs beginning with "http://" or "https://" will be parsed as a GS1
 * Digital Link URI during which the corresponding AI element string is
 * extracted and validated.
 *
 * EAN/UPC, GS1 DataBar and GS1-128 support a Composite Component. The
 * Composite Component must be specified in AI syntax. It must be separated
 * from the primary linear components with a "|" character and begin with an
 * FNC1 in first position, for example:
 *
 * \code
 * ^0112345678901231|^10ABC123^11210630
 * \endcode
 *
 * The above specifies a linear component representing "(01)12345678901231"
 * together with a composite component representing "(10)ABC123(11)210630".
 *
 * **Again, for GS1 data it is simpler and less error prone to specify the input
 * in human-friendly GS1 AI syntax using gs1_encoder_setAIdataStr().**
 *
 * \note
 * The length of the data must be less that the value returned by
 * gs1_encoder_getMaxDataStrLength().
 *
 * @see gs1_encoder_setAIdataStr()
 * @see gs1_encoder_getMaxDataStrLength()
 * @see gs1_encoder_getDataStr()
 * @see gs1_encoder_getErrMsg()
 * @see gs1_encoder_getErrMarkup()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] dataStr containing the raw barcode data
 * @return true on success, otherwise false and an error message is set that can be read using gs1_encoder_getErrMsg() and the errant AI data may be available using gs1_encoder_getErrMsg().
 */
GS1_ENCODERS_API bool gs1_encoder_setDataStr(gs1_encoder *ctx, const char *dataStr);


/**
 * @brief Sets the data in the buffer that is used when buffer input is
 * selected by parsing input provided in GS1 Application Identifier syntax into
 * a raw data string.
 *
 * The input is provided in human-friendly format **without** FNC1 characters
 * which are inserted automatically, for example:
 *
 * \code
 * (01)12345678901231(10)ABC123(11)210630
 * \endcode
 *
 * This syntax harmonises the format for the input accepted by all symbologies.
 * For example the following input is acceptable for EAN-13, UPC-A, UPC-E, any
 * variant of the GS1 DataBar family, GS1 QR Code and GS1 DataMatrix:
 *
 * \code
 * (01)00031234000054
 * \endcode
 *
 * The input is immediately parsed and validated against certain rules for GS1
 * AIs, after which the resulting encoding for valid inputs is available via
 * gs1_encoder_getDataStr(). If the input is invalid then this function will
 * return false and an error message will be set that can be read using
 * gs1_encoder_getErrMsg().
 *
 * Any "(" characters in AI element values must be escaped as "\(" to avoid
 * conflating them with the start of the next AI.
 *
 * For symbologies that support a composite component (all except
 * ::gs1_encoder_sDM, ::gs1_encoder_sQR and ::gs1_encoder_sDotCode), the data
 * for the linear and 2D components can be separated by a "|" character, for
 * example:
 *
 * \code
 * (01)12345678901231|(10)ABC123(11)210630
 * \endcode
 *
 * \note
 * The ultimate length of the encoded data must be less that the value returned by
 * gs1_encoder_getMaxDataStrLength().
 *
 * @see gs1_encoder_setDataStr()
 * @see gs1_encoder_getMaxDataStrLength()
 * @see gs1_encoder_getDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] dataStr the barcode input data in GS1 Application Identifier syntax
 * @return true on success, otherwise false and an error message is set
 */
GS1_ENCODERS_API bool gs1_encoder_setAIdataStr(gs1_encoder *ctx, const char *dataStr);


/**
 * @brief Return the barcode input data buffer in human-friendly AI syntax
 *
 * For example, if the input data buffer were to contain:
 *
 *     ^011231231231233310ABC123|^99XYZ(TM) CORP
 *
 * Then this function would return:
 *
 *     (01)12312312312333(10)ABC123|(99)XYZ\(TM) CORP
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to library
 * functions that modify the input data buffer such as
 * gs1_encoder_setDataStr(), gs1_encoder_setAIdataStr() and
 * gs1_encoder_setScanData().
 *
 * \note
 * The returned pointer should be checked for NULL which indicates non-AI input data.
 *
 * @see gs1_encoder_getDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return a pointer to a string representing the data input buffer in AI syntax, or a null pointer if the input data does not contain AI data
 */
GS1_ENCODERS_API char* gs1_encoder_getAIdataStr(gs1_encoder *ctx);


/**
 * @brief Returns a GS1 Digital Link URI representing AI-based input data.
 *
 * This example use of the library shows how to determine the GS1 Digital Link
 * representation for some bracketed AI data:
 *
 * \code
 * #include <stdio.h>
 * #include "gs1encoders.h"
 *
 * gs1_encoder *ctx = gs1_encoder_init(NULL);               // Create a new instance of the library
 * gs1_encoder_setAIdataStr(ctx,                            // Set the input data (AI format on this occasion)
 *        "(01)12345678901231(10)ABC123(11)210630");
 * printf("DL URI: %s\n", gs1_encoder_getDLuri(ctx,         // Print the GS1 Digital Link URI with a custom domain and stem
 *        "https://id.example.com/stem"));
 * gs1_encoder_free(ctx);                                   // Release the instance of the library
 * \endcode
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to library
 * functions that modify the input data buffer such as
 * gs1_encoder_setDataStr(), gs1_encoder_setAIdataStr() and
 * gs1_encoder_setScanData().
 *
 * \note
 * The returned pointer should be checked for NULL which indicates that invalid
 * input was provided for the selected symbology.
 *
 * @see gs1_encoder_setScanData()
 * @see gs1_encoder_setDataStr()
 * @see gs1_encoder_setAIdataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] stem a URI "stem" used as a prefix for the URI. If NULL, the GS1 canonical stem (`https://id.gs1.org/`) will be used.
 * @return a pointer to a string representing the GS1 Digital Link URI for the input data
 */
GS1_ENCODERS_API char* gs1_encoder_getDLuri(gs1_encoder *ctx, const char *stem);


/**
 * @brief Process normalised scan data received from a barcode reader with
 * reporting of AIM symbology identifiers enabled to extract the message data
 * and perform syntax checks in the case of GS1 Digital Link and AI data input.
 *
 * This function will process scan data (such as the output of a barcode
 * reader) and process the received data, setting the data input buffer to the
 * message received and setting the selected symbology to something that is
 * able to carry the received data.
 *
 * \note
 * In some instances the symbology determined by this library will not match
 * that of the image that was scanned. The AIM symbology identifier prefix of the
 * scan data does not always uniquely identify the symbology that was scanned.
 * For example GS1-128 Composite symbols share the same symbology identifier as
 * the GS1 DataBar family, and will therefore be detected as such.
 *
 * A literal "|" character may be included in the scan data to indicates the
 * separation between the first and second messages that would be transmitted
 * by a reader that is configured to return the composite component when
 * reading EAN/UPC symbols.
 *
 * This example use of the library processes a given scan data string, which is
 * assumed to represent AI data in this instance, and then renders the AI data
 * in human-friendly format.
 *
 * \code
 * #include <stdio.h>
 * #include "gs1encoders.h"
 *
 * gs1_encoder *ctx = gs1_encoder_init(NULL);                  // Create a new instance of the library
 * if (!gs1_encoder_setScanData(ctx,                           // Process the scan data, setting dataStr and Sym)
 *        "]C1011231231231233310ABC123" "\x1D" "99TESTING"))
 *     exit 1;                                                 // Handle failure if bad AI data is received
 * printf("AI data: %s\n", gs1_encoder_getAIdataStr(ctx));     // Print the AI scan data in human-friendly format
 * gs1_encoder_free(ctx);                                      // Release the instance of the library
 * \endcode
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to library
 * functions that modify the input data buffer such as
 * gs1_encoder_setDataStr(), gs1_encoder_setAIdataStr() and
 * gs1_encoder_setScanData().
 *
 * @see gs1_encoder_setScanData()
 * @see gs1_encoder_getDataStr()
 * @see gs1_encoder_getAIdataStr()
 * @see gs1_encoder_getSym()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [in] scanData the scan data input as read by a reader with AIM symbology identifiers enabled
 * @return true on success, otherwise false and an error message is set
 */
GS1_ENCODERS_API bool gs1_encoder_setScanData(gs1_encoder* ctx, const char *scanData);


/**
 * @brief Returns the string that should be returned by scanners when reading a
 * symbol that is an instance of the selected symbology and contains the same
 * input data.
 *
 * Examples:
 *
 * Symbology                  | Input data                                     | Returned scan data
 * -------------------------- | ---------------------------------------------- | -------------------------------------------------
 * ::gs1_encoder_sEAN13       | 2112345678900                                  | ]E02112345678900
 * ::gs1_encoder_sUPCA        | 416000336108                                   | ]E00416000336108
 * ::gs1_encoder_sEAN8        | 02345673                                       | ]E402345673
 * ::gs1_encoder_sEAN8        | 02345673\|^99COMPOSITE^98XYZ                   | ]E402345673\|]e099COMPOSITE{GS}98XYZ
 * ::gs1_encoder_sGS1_128_CCA | ^011231231231233310ABC123^99TESTING            | ]C1011231231231233310ABC123{GS}99TESTING
 * ::gs1_encoder_sGS1_128_CCA | ^0112312312312333\|^98COMPOSITE^97XYZ          | ]e00112312312312333{GS}98COMPOSITE{GS}97XYZ
 * ::gs1_encoder_sQR          | https://example.org/01/12312312312333          | ]Q1https://example.org/01/12312312312333
 * ::gs1_encoder_sQR          | ^01123123123123338200http://example.com        | ]Q301123123123123338200http://example.com
 * ::gs1_encoder_sDM          | https://example.com/gtin/09506000134352/lot/A1 | ]d1https://example.com/gtin/09506000134352/lot/A1
 * ::gs1_encoder_sDM          | ^011231231231233310ABC123^99TESTING            | ]d2011231231231233310ABC123{GS}99TESTING
 * ::gs1_encoder_sDotCode     | https://example.com/gtin/09506000134352/lot/A1 | ]J0https://example.com/gtin/09506000134352/lot/A1
 * ::gs1_encoder_sDotCode     | ^011231231231233310ABC123^99TESTING            | ]J1011231231231233310ABC123{GS}99TESTING
 *
 * The output will be prefixed with the appropriate AIM symbology identifier.
 *
 * "{GS}" in the scan data output in the above table represents a literal GS
 * character (ASCII 29) that is included in the returned data.
 *
 * The literal "|" character included in the scan data output for EAN/UPC
 * Composite symbols indicates the separation between the first and second
 * messages that would be transmitted by a reader that is configured to return
 * the composite component.
 *
 * This example use of the library shows how to determine what data a scanner
 * should provide when reading a certain symbol:
 *
 * \code
 * #include <stdio.h>
 * #include "gs1encoders.h"
 *
 * gs1_encoder *ctx = gs1_encoder_init(NULL);               // Create a new instance of the library
 * gs1_encoder_setSym(ctx, gs1_encoder_sDataBarExpanded);   // Choose the symbology
 * gs1_encoder_setAIdataStr(ctx,                            // Set the input data (AI format on this occasion)
 *        "(01)12345678901231(10)ABC123(11)210630");
 * printf("Scan data: %s\n", gs1_encoder_getScanData(ctx)); // Print the scan data that a reader should return
 * gs1_encoder_free(ctx);                                   // Release the instance of the library
 * \endcode
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to library
 * functions that modify the input data buffer such as
 * gs1_encoder_setDataStr(), gs1_encoder_setAIdataStr() and
 * gs1_encoder_setScanData().
 *
 * @see gs1_encoder_setScanData()
 * @see gs1_encoder_setDataStr()
 * @see gs1_encoder_setAIdataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return a pointer to a string representing the scan data for the input data contained within symbols of the selected symbology
 */
GS1_ENCODERS_API char* gs1_encoder_getScanData(gs1_encoder* ctx);


/**
 * @brief Update a given pointer towards an array of strings containing
 * Human-Readable Interpretation ("HRI") text.
 *
 * For example, if the input data buffer were to contain:
 *
 *     ^011231231231233310ABC123|^99XYZ(TM) CORP
 *
 * Then this function would return the following array of null-terminated
 * strings:
 *
 *     "(01) 12312312312333"
 *     "(10) ABC123"
 *     "--"
 *     "(99) XYZ(TM) CORP"
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to functions
 * that modify the input data buffer such as gs1_encoder_setDataStr(),
 * gs1_encoder_setAIdataStr() or gs1_encoder_setScanData().
 *
 * @see gs1_encoder_getDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [out] hri Pointer to an array of HRI strings
 * @return the number of HRI strings
 */
GS1_ENCODERS_API int gs1_encoder_getHRI(gs1_encoder* ctx, char ***hri);


/**
 * @brief Get the require HRI buffer size.
 *
 * @deprecated Use gs1_encoder_getHRI() instead.
 *
 * @see gs1_encoder_copyHRI()
 *
 * \note
 * This function is useful when used in tandem with gs1_encoder_copyHRI() to
 * determine the size of the user-provided buffer to pre-allocate.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return required length of the buffer
 */
GS1_ENCODERS_API DEPRECATED size_t gs1_encoder_getHRIsize(gs1_encoder *ctx);


/**
 * @brief Copy the HRI to a user-provided buffer in the form of a "|"-separated string.
 *
 * @deprecated Use gs1_encoder_getHRI() instead.
 *
 * The buffer into which the output buffer is copied must be preallocated with
 * at least the size returned by gs1_encoder_getHRIsize().
 *
 * @see gs1_encoder_getHRIsize()
 *
 * \note
 * This function exists mainly for environments where it is difficult to access
 * library-allocated buffers via a pointer modified in function parameters.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [out] buf a pointer to a buffer into which the formatted HRI text is copied
 * @param [in] max the maximum number of bytes that may be copied into the provided buffer
 */
GS1_ENCODERS_API DEPRECATED void gs1_encoder_copyHRI(gs1_encoder *ctx, void *buf, size_t max);


/**
 * @brief Update a given pointer towards an array of strings containing
 * the non-numeric (ignored) query parameters in a provided GS1 Digital Link
 * URI.
 *
 * For example, if the input data buffer were to contain:
 *
 *     https://a/01/12312312312333/22/ABC?name=Donald%2dDuck&99=ABC&testing&type=cartoon
 *
 * Then this function would return the following array of null-terminated
 * strings:
 *
 *     "name=Donald%2dDuck"
 *     "testing"
 *     "type=cartoon"
 *
 * \note
 * The returned strings are not URI decoded. The expected use for the function
 * is to present which sections of a given GS1 Digital Link URI have been
 * ignored.
 *
 * \note
 * The return data does not need to be free()ed and the content should be
 * copied if it must persist in user code after subsequent calls to functions
 * that modify the input data buffer such as gs1_encoder_setDataStr(),
 * gs1_encoder_setAIdataStr() or gs1_encoder_setScanData().
 *
 * @see gs1_encoder_getDataStr()
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [out] qp Pointer to an array of non-numeric (ignored) query parameters
 * @return the number of ignored query parameters
 */
GS1_ENCODERS_API int gs1_encoder_getDLignoredQueryParams(gs1_encoder* ctx, char ***qp);


/**
 * @brief Get the require buffer size for ignored GS1 Digital Link query
 * parameters.
 *
 * @deprecated Use gs1_encoder_getDLignoredQueryParams() instead.
 *
 * @see gs1_encoder_copyDLignoredQueryParams()
 *
 * \note
 * This function is useful when used in tandem with
 * gs1_encoder_copyDLignoredQueryParams() to determine the size of the
 * user-provided buffer to pre-allocate.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @return required length of the buffer
 */
GS1_ENCODERS_API DEPRECATED size_t gs1_encoder_getDLignoredQueryParamsSize(gs1_encoder *ctx);


/**
 * @brief Copy the non-numeric (ignored) GS1 Digital Link query parameters to a
 * user-provided buffer in the form of a "&"-separated string.
 *
 * @deprecated Use gs1_encoder_getDLignoredQueryParams() instead.
 *
 * The buffer into which the output buffer is copied must be preallocated with
 * at least the size returned by gs1_encoder_getDLignoredQueryParamsSize().
 *
 * @see gs1_encoder_getDLignoredQueryParamsSize()
 *
 * \note
 * This function exists mainly for environments where it is difficult to access
 * library-allocated buffers via a pointer modified in function parameters.
 *
 * @param [in,out] ctx ::gs1_encoder context
 * @param [out] buf a pointer to a buffer into which the formatted non-numeric (ignored) query parameters are copied
 * @param [in] max the maximum number of bytes that may be copied into the provided buffer
 */
GS1_ENCODERS_API DEPRECATED void gs1_encoder_copyDLignoredQueryParams(gs1_encoder *ctx, void *buf, size_t max);


/**
 *  @brief Destroy a ::gs1_encoder instance.
 *
 *  This will release all library-managed storage associated with the instance.
 *
 *  @param [in,out] ctx ::gs1_encoder context to destroy
 */
GS1_ENCODERS_API void gs1_encoder_free(gs1_encoder *ctx);


#ifdef __cplusplus
}
#endif


#undef GS1_ENCODERS_API
#undef DEPRECATED


#endif /* GS1_ENCODERS_H */
