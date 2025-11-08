/**
 * @module gs1encoder
 *
 * @description
 *
 * <h1>GS1 Barcode Syntax Engine - JS + WASM wrapper</h1>
 *
 * <p>
 * The GS1 Barcode Syntax Engine provides routines that support the processing
 * of GS1 syntax data, including Application Identifier element strings and GS1
 * Digital Link URIs, whether these are provided in raw or human-friendly
 * format or as normalised scan data received from barcode readers.
 *
 * <p>
 * The implementations are intended for use with GS1 standards and applications
 * and do not contain additional features that might be required for more
 * general use.
 *
 * <p>
 * Within the GS1 Application Identifier system, structured data is represented
 * in different formats depending upon the context.
 *
 * <p>
 * The data formats supported by this library are:
 *
 * <ul>
 * <li><strong>Bracketed AI element strings</strong>: Human-friendly rendition of AI data as a single string using numerical AIs.
 * <li><strong>Unbracketed AI element strings</strong>: Rendition of AI data that corresponds most directly to encoded barcode data.
 * <li><strong>GS1 Digital Link URIs</strong>
 * <li><strong>Scan data</strong>: The expected result of scanning a symbol with a barcode reader that has AIM symbologies identifiers enabled.
 * <li><strong>Human Readable Interpretation (HRI)</strong>: Human-friendly redition of the AI data contained within a symbol. This may also include Data Titles to present the AI data in the form of "mixed HRI/non-HRI text". (Output only.)
 * </ul>
 *
 * <p>
 * The following diagram shows how the library can be used for processing and
 * transformation of GS1 data, indicating which formats are accepted as input,
 * how barcode message data is generated and AI data extracted from the
 * provided input data, and how the given data can be output in various
 * formats.
 *
 * <p>
 * <img src="../input_output_buffers.svg" alt="Data transformation: Inputs, outputs and buffers" style="max-width: 100%;">
 *
 * <p>
 * The above diagram highlights that conceptually the library contains two internal "data buffers":
 *
 * <ul>
 * <li><strong>Barcode message buffer:</strong> This is populated with the raw message
 *     that would be borne by a GS1 barcode symbol that represents the input
 *     data, e.g. unbracketed AI syntax with FNC1 in first for regular AI
 *     element strings; plain string for a plain data or a GS1 Digital Link
 *     URI.
 * <li><strong>Extracted AI buffer:</strong> This contains the in-order AI data that was
 *     extracted from the input data.
 * </ul>
 *
 * <p>
 * The main operations of the library involve reading and updating the state of
 * these buffers.
 *
 *
 * <h2 id="quick-start">Quick start</h2>
 *
 * <h3>Library installation</h3>
 *
 * The wrapper and WASM are provided in the form of an NPM module which can be
 * downloaded as follows:
 *
 * <pre>
 * npm init es6
 * npm install gs1encoder
 * </pre>
 *
 *
 * <h3>Minimal Node.js CLI example</h3>
 *
 * For a minimal CLI example that verifies that the library is working create a
 * file called <code>app.mjs</code> as follows:
 *
 * <pre>
 * import { GS1encoder } from "gs1encoder";
 *
 * const gs = new GS1encoder();
 * await gs.init();
 *
 * console.log("Version:", gs.version);
 *
 * gs.free();
 * </pre>
 *
 * You can then run the example with:
 *
 * <pre>
 * node app.mjs
 * </pre>
 *
 * For a comprehensive Node.js interactive example, see
 * <a href="https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.node.mjs">example.node.mjs</a>.
 *
 *
 * <h3 id="quick-start-browser">Minimal browser example</h3>
 *
 * For browser usage in addition to initialising the library, create a
 * <code>index.html</code> file as follows:
 *
 * <pre>
 * &lt;!DOCTYPE html&gt;
 * &lt;html&gt;
 * &lt;head&gt;
 *     &lt;meta charset="utf-8"&gt;
 *     &lt;title&gt;GS1 Encoder Test&lt;/title&gt;
 * &lt;/head&gt;
 * &lt;body&gt;
 *     &lt;h1&gt;GS1 Encoder Version Test&lt;/h1&gt;
 *
 *     &lt;div id="output"&gt;&lt;/div&gt;
 *
 *     &lt;script type="module"&gt;
 *         import { GS1encoder } from './node_modules/gs1encoder/gs1encoder.mjs';
 *
 *         const gs = new GS1encoder();
 *         await gs.init();
 *
 *         document.getElementById('output').textContent = 'Version: ' + gs.version;
 *
 *         gs.free();
 *     &lt;/script&gt;
 * &lt;/body&gt;
 * &lt;/html&gt;
 * </pre>
 *
 * Then serve it with:
 *
 * <pre>
 * npx serve
 * </pre>
 *
 * Point a web browser at the links emitted by the above command.
 *
 * <p>
 * For a comprehensive broswer example, see
 * <a href="https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.html">example.html</a>
 * with
 * <a href="https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.mjs">example.mjs</a>.
 *
 *
 * <h2>Example uses</h2>
 *
 * The following are examples of how to use the library.
 *
 * <p>
 * Using the library always begins by creating a new GS1encoder instance
 * and calling {@link GS1encoder#init}.
 *
 * <p>
 * Unless otherwise specified, the getter methods return library-managed
 * data that must not be modified by the user. If their content must persist
 * following a subsequent call to the same instance of the library then they
 * must be copied to a user-managed variable.
 *
 * <p>
 * Most of the setter and action methods of this library throw exceptions
 * in the event of failure. Production code should catch these exceptions and
 * handle them appropriately, which might include rendering the error message to
 * the user.
 *
 *
 * <h3>GS1 AI data validation and extraction (including GS1 Digital Link)</h3>
 *
 * The following code processes AI data input, validates it (reporting any
 * failures) and displays the extracted AIs if the validation succeeds.
 *
 * <pre>
 * const gs = new GS1encoder();                             // Create a new instance of the library
 * await gs.init();
 *
 * // gs.permitUnknownAIs = true;                           // Uncomment only if it is necessary to handle AIs
 *                                                          // that are not known to the library
 *
 * // Input provided as a bracketed AI element string
 * //
 * try {
 *     gs.aiDataStr = "(01)12312312312333(10)ABC123(99)TEST";
 * } catch (e) {
 *     console.error("ERROR: " + e.message);                // Display a descriptive error message
 *     const errMarkup = gs.errMarkup;
 *     if (errMarkup !== '')                                // Display the invalid AI in the case of a Linting failure
 *         console.error("Bad AI data: " + errMarkup);
 *     // Finally, handle the error in an application-specific way
 *     throw e;
 * }
 *
 * // Alternatively, the input may be given in the following formats:
 * //
 * // gs.dataStr = "^011231231231233310ABC123^99TEST";      // Unbracketed element string, "^" = FNC1
 * //
 * // gs.dataStr = "https://example.com/01/12312312312333/10/ABC123/99/TEST";   // GS1 Digital Link URI
 * //
 * // gs.scanData = "]Q1011231231231233310ABC123\u001D99TEST";   // Barcode scan data, containing a "GS" (ASCII 0x1D) separator
 *
 * for (const line of gs.getHRI()) {                        // Display the extracted AI data as HRI text
 *     console.log(line);
 * }
 * </pre>
 *
 *
 * <h3>Converting an AI element string to barcode message data</h3>
 *
 * In this example we process a bracketed AI element string to convert it into
 * barcode message data, suitable for carrying in a GS1 barcode symbol.
 *
 * <pre>
 * const gs = new GS1encoder();
 * await gs.init();
 *
 * try {
 *     gs.aiDataStr = "(01)12312312312333(10)ABC123(99)TEST";    // Accept a bracketed AI element string
 * } catch (e) {
 *     // Handle error and return
 * }
 *
 * console.log(gs.dataStr);                                 // Render the barcode message buffer
 * </pre>
 *
 * <p>
 * <strong>Note:</strong> The barcode message data read and emitted by this library uses a harmonised
 * format that does not concern itself with internal encoding quirks of various
 * symbologies. In the harmonised barcode message data:
 *
 * <ul>
 * <li>A leading <code>"^"</code> always indicates GS1 Application Identifier syntax data,
 *   i.e. a notional FNC1 in first character position. (This is even true for
 *   DotCode in whose <em>internal encoding</em> the literal FNC1 non-data character
 *   may have an inverted meaning for certain messages depending upon their
 *   regular data content.)
 * <li>A <code>"^"</code> at any other position represents a notional FNC1 non-data
 *   Application Identifier separator character. (This is even the case for QR
 *   Code in whose <em>internal encoding</em> a <code>"%"</code> character or <code>"{GS}"</code> character
 *   takes on the AI separator role typically assigned to the FNC1 non-data
 *   character, depending upon the effectuve encodation mode.)
 * </ul>
 *
 * <p>
 * Additionally, barcode image encoder libraries have differing conventions for
 * how to input FNC1 characters, extending to whether it is necessary to be
 * explicit about the notional FNC1 character in the first position when
 * specifying a GS1 Application Identifier syntax symbol.
 *
 * <p>
 * Consequently, the barcode message data emitted by this library may need to
 * be post-processed to align to the specific requirements of whatever symbol
 * generation library is in use.
 *
 *
 * <h3>Barcode scan data processing</h3>
 *
 * In this example we process scan data from a barcode reader to extract the AI
 * data.
 *
 * <pre>
 * const gs = new GS1encoder();
 * await gs.init();
 *
 * // Disable validation of mandatory association between AIs if the symbol may
 * // be one of multiple on a label
 * gs.setValidationEnabled(validation.RequisiteAIs, false);
 *
 * try {
 *     gs.scanData = "]Q1011231231231233310ABC123\u001D99TEST";
 * } catch (e) {
 *     // Handle error and return
 * }
 *
 * for (const line of gs.getHRI()) {
 *     console.log(line);
 * }
 *
 * // If it is necessary to know the "symbology" that was scanned then this can
 * // be read using sym, however note the caveats given in the
 * // description of scanData
 * </pre>
 *
 * <p>
 * <strong>Note:</strong> It is required that AIM Symbology Identifiers are enabled on the barcode
 * reader, and that the scanned barcode message "survives the channel" intact,
 * i.e. that no character substitutions have been made by the reader, in
 * particular that any embedded FNC1 separator characters are correctly
 * represented by GS characters (ASCII 29). If this is not the case then the
 * scanned data should be pre-processed to meet this requirement.
 *
 */
