/**
 * <h2>GS1 Barcode Syntax Engine - Java wrapper.</h2>
 *
 * <p>
 * The GS1 Barcode Syntax Engine provides routines that support the processing of GS1
 * syntax data, including Application Identifier element strings and GS1
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
 * <img src="../../../input_output_buffers.svg" alt="Data transformation: Inputs, outputs and buffers" style="max-width: 1000px; width: 100%;">
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
 * To use the library you must first build the wrapper for your platform.
 *
 * <h3>Building the Java wrapper</h3>
 *
 * <strong>On Unix/macOS:</strong>
 *
 * <ol>
 * <li>First, compile the C library as a static library:
 * <pre>
 * make -C src/c-lib -j `nproc` libstatic
 * </pre>
 * </li>
 * <li>Then build the Java wrapper:
 * <pre>
 * ant -f src/java/build.xml
 * </pre>
 * </li>
 * </ol>
 *
 * <strong>On Windows:</strong>
 *
 * <ol>
 * <li>First, build the C library using Visual Studio or from a Developer Command Prompt:
 * <pre>
 * msbuild src\gs1encoders.sln /t:gs1encoders /p:Configuration=Debug /p:Platform=x64
 * </pre>
 * </li>
 * <li>Then build the Java wrapper:
 * <pre>
 * ant -f src\java\build.xml
 * </pre>
 * </li>
 * </ol>
 *
 * This generates two files in {@code src/java/}:
 *
 * <ul>
 * <li>{@code libgs1encoders.jar} - The Java class library
 * <li>{@code libgs1encodersjni.so} (or .dll on Windows, .dylib on macOS) - The native JNI library
 * <li>{@code libgs1encoders.so} symlink (or .dll on Windows, .dylib on macOS)
 * </ul>
 *
 *
 * <h3>Running the example application</h3>
 *
 * After building, you can test it by running the interactive example
 * application:
 *
 * <pre>
 * ant -f src/java/build.xml test
 * </pre>
 *
 * The example application provides an interactive menu for testing different
 * input formats and features.
 *
 *
 * <h3>Using in Your Own Project</h3>
 *
 * To use the wrapper in your Java project you must:
 *
 * <ol>
 * <li>Add {@code libgs1encoders.jar} to your project's classpath at compile time and runtime
 * <li>Ensure the directory containing the native library ({@code libgs1encodersjni.so}) and the symlink ({@code libgs1encoders.so}) are in {@code java.library.path} at runtime
 * </ol>
 *
 * For a minimal example a {@code MyApp.java} file as follows:
 *
 * <pre>
 * import org.gs1.gs1encoders.*;
 *
 * public class MyApp {
 *
 *     static {
 *         System.loadLibrary("gs1encodersjni");
 *     }
 *
 *     public static void main(String[] args) {
 *         try {
 *             GS1Encoder gs = new GS1Encoder();
 *             System.out.println("GS1 Syntax Engine version: " + gs.getVersion());
 *             gs.free();
 *         } catch (Exception e) {
 *             System.err.println("Error: " + e.getMessage());
 *         }
 *     }
 * }
 * </pre>
 *
 * <strong>On Unix/macOS:</strong>
 *
 * <p>
 * Compile your application:
 *
 * <pre>
 * javac -classpath &lt;path-to-gs1-syntax-engine&gt;/src/java/libgs1encoders.jar MyApp.java
 * </pre>
 *
 * Run your application:
 *
 * <pre>
 * java -classpath &lt;path-to-gs1-syntax-engine&gt;/src/java/libgs1encoders.jar:. -Djava.library.path=&lt;path-to-gs1-syntax-engine&gt;/src/java MyApp
 * </pre>
 *
 * <strong>On Windows:</strong>
 *
 * <p>
 * Compile your application:
 *
 * <pre>
 * javac -classpath &lt;path-to-gs1-syntax-engine&gt;\src\java\libgs1encoders.jar MyApp.java
 * </pre>
 *
 * Run your application:
 *
 * <pre>
 * java -classpath &lt;path-to-gs1-syntax-engine&gt;\src\java\libgs1encoders.jar;. -Djava.library.path=&lt;path-to-gs1-syntax-engine&gt;\src\java MyApp
 * </pre>
 *
 * For a comprehensive example, see <a href="https://github.com/gs1/gs1-syntax-engine/blob/main/src/java/Example.java">Example.java</a>.
 *
 *
 * <h3 id="android-mobile-app-quick-start">Android Mobile App Quick Start</h3>
 *
 * To use the GS1 Barcode Syntax Engine in an Android application:
 *
 * <ol>
 * <li>Create a new Android project in Android Studio (File → New → New Project).
 * <li>When prompted to select a template, choose Phone and Tablet → Native C++.
 * <li>Create symlinks to the Java sources, wrapper source, and C library sources:
 *   <p>
 *   <strong>On Unix/macOS:</strong>
 *   <pre>
 * cd &lt;project-root&gt;/app/src/main/java
 * mkdir -p org/gs1
 * cd org/gs1
 * ln -s &lt;path-to-gs1-syntax-engine&gt;/src/java/org/gs1/gs1encoders gs1encoders
 *
 * cd &lt;project-root&gt;/app/src/main/cpp
 * ln -s &lt;path-to-gs1-syntax-engine&gt;/src/c-lib gs1encoders
 * ln -s &lt;path-to-gs1-syntax-engine&gt;/src/java/gs1encoders_wrap.c gs1encoders_wrap.c
 *   </pre>
 *   <strong>On Windows:</strong>
 *   <p>
 *   Open Command Prompt, then:
 *   <pre>
 * cd &lt;project-root&gt;\app\src\main\java
 * mkdir org\gs1
 * cd org\gs1
 * mklink /D gs1encoders &lt;path-to-gs1-syntax-engine&gt;\src\java\org\gs1\gs1encoders
 *
 * cd &lt;project-root&gt;\app\src\main\cpp
 * mklink /D gs1encoders &lt;path-to-gs1-syntax-engine&gt;\src\c-lib
 * mklink gs1encoders_wrap.c &lt;path-to-gs1-syntax-engine&gt;\src\java\gs1encoders_wrap.c
 *   </pre>
 *   <p>
 *   <em>Ensure that Window's Developer Mode is enabled unless you are running as Administrator.</em>
 * <li>Append the following to {@code app/src/main/cpp/CMakeLists.txt}:
 *   <pre>
 * file(GLOB LIB_SOURCE_FILES
 *     gs1encoders/ai.c
 *     gs1encoders/dl.c
 *     gs1encoders/scandata.c
 *     gs1encoders/syn.c
 *     gs1encoders/gs1encoders.c
 *     gs1encoders/syntax/gs1syntaxdictionary.c
 *     gs1encoders/syntax/lint_*.c
 *     gs1encoders_wrap.c
 * )
 *
 * include_directories(gs1encoders)
 * add_compile_definitions(GS1_LINTER_ERR_STR_EN)
 * add_library(gs1encoders SHARED ${LIB_SOURCE_FILES})
 *   </pre>
 * <li>Amend your Activity to import the gs1encoders package, load the native library, and then use it:
 *   <pre>
 * ...
 * import org.gs1.gs1encoders.GS1Encoder
 *
 * class MainActivity : AppCompatActivity() {
 *
 *     companion object {
 *         init {
 *             System.loadLibrary("gs1encoders")
 *         }
 *     }
 *
 *     private lateinit var gs: GS1Encoder
 *
 *     override fun onCreate(savedInstanceState: Bundle?) {
 *         super.onCreate(savedInstanceState)
 *         setContentView(R.layout.activity_main)
 *
 *         gs = GS1Encoder()
 *         val version = gs.version
 *         title = "GS1 Encoders library: $version"
 *
 *         // Use gs to process barcode data...
 *
 *     }
 *
 *     override fun onDestroy() {
 *         super.onDestroy()
 *         gs.free()
 *     }
 * }
 *   </pre>
 * <li>In Android Studio, reload the Gradle configuration by clicking <strong>File → Sync Project with Gradle Files</strong> (or click the "Sync" icon in the toolbar).
 * <li>Build the project by clicking <strong>Build → Make Project</strong> (or press Ctrl+F9 / Cmd+F9).
 * <li>Run the app by clicking <strong>Run → Run 'app'</strong> (or click the green play button, or press Shift+F10).
 * </ol>
 *
 * For a comprehensive Android app example with barcode scanning and full UI, see the
 * <a href="https://github.com/gs1/gs1-syntax-engine/tree/main/src/android">GS1 Encoders Android App</a>.
 *
 *
 * <h2 id="example-uses">Example uses</h2>
 *
 * The following are examples of how to use the library.
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
 * GS1Encoder gs = new GS1Encoder();                           // Create a new instance of the library
 *
 * // gs.setPermitUnknownAIs(true);                            // Uncomment only if it is necessary to handle AIs
 *                                                             // that are not known to the library
 *
 * // Input provided as a bracketed AI element string
 * //
 * try {
 *     gs.setAIdataStr("(01)12312312312333(10)ABC123(99)TEST");
 * } catch (Exception e) {
 *     System.err.println("ERROR: " + e.getMessage());         // Display a descriptive error message
 *     String errMarkup = gs.getErrMarkup();
 *     if (!errMarkup.isEmpty())                               // Display the invalid AI in the case of a Linting failure
 *         System.err.println("Bad AI data: " + errMarkup);
 *     // Finally, handle the error in an application-specific way
 *     System.exit(1);
 * }
 *
 * // Alternatively, the input may be given in the following formats:
 * //
 * // gs.setDataStr("^011231231231233310ABC123^99TEST");       // Unbracketed element string, "^" = FNC1
 * //
 * // gs.setDataStr("https://example.com/01/12312312312333/10/ABC123/99/TEST");   // GS1 Digital Link URI
 * //
 * // gs.setScanData("]Q1011231231231233310ABC123\u001D99TEST");   // Barcode scan data, containing a "GS" (ASCII 0x1D) separator
 *
 * for (String line : gs.getHRI()) {                           // Display the extracted AI data as HRI text
 *     System.out.println(line);
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
 * GS1Encoder gs = new GS1Encoder();
 *
 * try {
 *     gs.setAIdataStr("(01)12312312312333(10)ABC123(99)TEST");    // Accept a bracketed AI element string
 * } catch (Exception e) {
 *     // Handle error and return
 * }
 *
 * System.out.println(gs.getDataStr());                        // Render the barcode message buffer
 * </pre>
 *
 * <p>
 * <strong>Note:</strong> The barcode message data read and emitted by this library uses a harmonised
 * format that does not concern itself with internal encoding quirks of various
 * symbologies. In the harmonised barcode message data:
 *
 * <ul>
 * <li>A leading {@code "^"} always indicates GS1 Application Identifier syntax data,
 *   i.e. a notional FNC1 in first character position. (This is even true for
 *   DotCode in whose <em>internal encoding</em> the literal FNC1 non-data character
 *   may have an inverted meaning for certain messages depending upon their
 *   regular data content.)
 * <li>A {@code "^"} at any other position represents a notional FNC1 non-data
 *   Application Identifier separator character. (This is even the case for QR
 *   Code in whose <em>internal encoding</em> a {@code "%"} character or {@code "{GS}"} character
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
 * GS1Encoder gs = new GS1Encoder();
 *
 * // Disable validation of mandatory association between AIs if the symbol may
 * // be one of multiple on a label
 * gs.setValidationEnabled(Validation.RequisiteAIs, false);
 *
 * try {
 *     gs.setScanData("]Q1011231231231233310ABC123\u001D99TEST");
 * } catch (Exception e) {
 *     // Handle error and return
 * }
 *
 * for (String line : gs.getHRI()) {
 *     System.out.println(line);
 * }
 *
 * // If it is necessary to know the "symbology" that was scanned then this can
 * // be read using getSym(), however note the caveats given in the
 * // description of setScanData()
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
 * @author GS1 AISBL
 *
 */
package org.gs1.gs1encoders;
