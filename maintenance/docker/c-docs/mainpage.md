## GS1 Barcode Syntax Engine {#mainpage}

### Overview

The GS1 Barcode Syntax Engine provides routines that support the processing of GS1
syntax data, including Application Identifier element strings and GS1
Digital Link URIs, whether these are provided in raw or human-friendly
format or as normalised scan data received from barcode readers.

The implementations are intended for use with GS1 standards and applications
and do not contain additional features that might be required for more
general use.

Within the GS1 Application Identifier system, structured data is represented
in different formats depending upon the context.

The data formats supported by this library are:

  * **Bracketed AI element strings**: Human-friendly rendition of AI data as a single string using numerical AIs.
  * **Unbracketed AI element strings**: Rendition of AI data that corresponds most directly to encoded barcode data.
  * **GS1 Digital Link URIs**
  * **Scan data**: The expected result of scanning a symbol with a barcode reader that has AIM symbologies identifiers enabled.
  * **Human Readable Interpretation (HRI)**: Human-friendly redition of the AI data contained within a symbol. This may also include Data Titles to present the AI data in the form of "mixed HRI/non-HRI text". (Output only.)

This following diagram shows how the library can be used for processing and
transformation of GS1 data, indicating which formats are accepted as input,
how barcode message data is generated and AI data extracted from the
provided input data, and how the given data can be output in various
formats.

| Data transformation: Inputs, outputs and buffers |
| :----------------------------------------------- |
| \image html input_output_buffers.svg             |

The above diagram highlights that conceptually the library contains two
internal "data buffers":

  * **Barcode message buffer:** This is populated with the raw message
    that would be borne by a GS1 barcode symbol that represents the input
    data, e.g. unbracketed AI syntax with FNC1 in first for regular AI
    element strings; plain string for a plain data or a GS1 Digital Link
    URI.

  * **Extracted AI buffer:** This contains the in-order AI data that was
    extracted from the input data.

The main operations of the library involve reading and updating the state of
these buffers.

### Example Uses

The following are examples of how to use the library.

\note
Using the library always begins by initialising the library with
@ref gs1_encoder_init() or @ref gs1_encoder_init_ex() and finishes by releasing the
library with @ref gs1_encoder_free().

\note
Unless otherwise specified, the getter functions return pointers to
per-instance storage managed by this library and therefore must not be freed
by the user. If their content must persist following a subsequent call to
the same instance of the library then they must be copied to a user-managed
buffer.

\note
Most of the setter and action functions of this library return a boolean
indicating whether the function was successful and write an error message
that can be accessed with @ref gs1_encoder_getErrMsg() in the event of failure.
Production code should check the output of the functions and where relevant
do something appropriate which might include rendering the error message to
the user. Error message string are provided in the English language in a
single file that can be replaced at compile time.

Refer to the example console application (`gs1encoders-app.c`) for a
comprehensive example of how to use this library.


#### GS1 AI data validation and extraction (including GS1 Digital Link)

The following code processes AI data input, validates it (reporting any
failures) and displays the extracted AIs if the validation succeeds.

\code
gs1_encoder *ctx = gs1_encoder_init(NULL);              // Create a new instance of the library

// gs1_encoder_permitUnknownAIs(ctx, true);             // Uncomment only if it is necessary to handle AIs
                                                        // that are not known to the library

// Input provided as a bracketed AI element string
//
bool ret = gs1_encoder_setAIdataStr(ctx, "(01)12312312312333(10)ABC123(99)TEST");

// Alternatively, the input may be given in the following formats:
//
// bool ret = gs1_encoder_setDataStr(ctx,               // Unbracketed element string, "^" = FNC1
//                "^011231231231233310ABC123^99TEST");
//
// bool ret = gs1_encoder_setDataStr(ctx,               // GS1 Digital Link URI
//                "https://example.com/01/12312312312333/10/ABC123/99/TEST");
//
// bool ret = gs1_encoder_setScanData(ctx,              // Barcode scan data, containing a "GS" (ASCII 0x1D) separator
//                "]Q1011231231231233310ABC123" "\x1D" "99TEST");

if (!ret) {
    printf("ERROR: %s\n", gs1_encoder_getErrMsg(ctx));  // Display a descriptive error message
    char *errMarkup = gs1_encoder_getErrMarkup(ctx);
    if (*errMarkup != '\0')                             // Display the invalid AI in the case of a Linting failure
        printf("Bad AI data: %s\n", errMarkup);
    abort();                                            // Finally, handle the error in an application-specific way
}

char **hri;
int numHRI = gs1_encoder_getHRI(ctx, &hri);             // Display the extracted AI data as HRI text
for (int i = 0; i < numHRI; i++) {
    printf("%s\n", hri[i]);
}

gs1_encoder_free(ctx);                                  // Release the instance of the library
\endcode


#### Converting an AI element string to barcode message data

In this example we process a bracketed AI element string to convert it into
barcode message data, suitable for carrying in a GS1 barcode symbol.

\code
gs1_encoder *ctx = gs1_encoder_init(NULL);

bool ret = gs1_encoder_setAIdataStr(ctx,        // Accept a bracketed AI element string
    "(01)12312312312333(10)ABC123(99)TEST");

if (!ret) {
    // Handle error and return
}

printf("%s\n", gs1_encoder_getDataStr(ctx));    // Render the barcode message buffer

gs1_encoder_free(ctx);
\endcode

\note
The barcode message data read and emitted by this library uses a harmonised
format that does not concern itself with internal encoding quirks of various
symbologies. In the harmonised barcode message data:
\note
  - A leading `^` always indicates GS1 Application Identifier syntax data,
  i.e. a notional FNC1 in first character position. (This is even true for
  DotCode in whose *internal encoding* the literal FNC1 non-data character
  may have an inverted meaning for certain messages depending upon their
  regular data content.)
  - A `^` at any other position represents a notional FNC1 non-data
  Application Identifier separator character. (This is even the case for QR
  Code in whose *internal encoding* a `%` character or `{GS}` character
  takes on the AI separator role typically assigned to the FNC1 non-data
  character, depending upon the effectuve encodation mode.)
\note
Additionally, barcode image encoder libraries have differing conventions for
how to input FNC1 characters, extending to whether it is necessary to be
explicit about the notional FNC1 character in the first position when
specifying a GS1 Application Identifier syntax symbol.
\note
Consequently, the barcode message data emitted by this library may need to
be post-processed to align to the specific requirements of whatever symbol
generation library is in use.


#### Barcode scan data processing

In this example we process scan data from a barcode reader to extract the AI
data.

\code
gs1_encoder *ctx = gs1_encoder_init(NULL);

// Disable validation of mandatory association between AIs if the symbol may
// be one of multiple on a label
setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, false);

bool ret = gs1_encoder_setScanData(ctx,
               "]Q1011231231231233310ABC123" "\x1D" "99TEST");

if (!ret) {
    // Handle error and return
}

char **hri;
int numHRI = gs1_encoder_getHRI(ctx, &hri);
for (int i = 0; i < numHRI; i++) {
    printf("%s\n", hri[i]);
}

// If it is necessary to know the "symbology" that was scanned then this can
// be read using gs1_encoder_getSym(), however note the caveats given in the
// description of gs1_encoder_setScanData()

gs1_encoder_free(ctx);
\endcode

\note
It is required that AIM Symbology Identifiers are enabled on the barcode
reader.

\note
It is assumed the scanned barcode message "survives the channel" intact,
i.e. that no character substitutions have been made by the reader, in
particular that any embedded FNC1 separator characters are correctly
represented by GS characters (ASCII 29). If this is not the case then the
scanned data should be pre-processed to meet this requirement.


### Compile-Time Configuration Macros

The following macros may be defined at compile time to enable or disable
optional features:

`GS1_ENCODERS_ERR_LANG=<LANG>`
:  Used to specify alternative translation strings for error messages.
   Create a new header file named `tr_<LANG>.h` based on the existing
   `tr_EN.h` file.

`EXCLUDE_EMBEDDED_AI_TABLE`
:  Excludes the embedding a table of AI properties in order to reduce the
   size of the library. The table of AI definitions must be populated by
   parsing from the GS1 Syntax Dictionary instead, without falling back to
   the embedded instance.

`EXCLUDE_SYNTAX_DICTIONARY_LOADER`
:  Excludes functions for populating the table of AIs by parsing the GS1
   Syntax Dictionary. The AI definitions must be embedded for the library to
   function, i.e. this option is mutually exclusive of
   `EXCLUDE_EMBEDDED_AI_TABLE`.

`GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H=<CUSTOM_HEADER.h>`
:  Points to a file that declares alternative heap management routines via
   the `GS1_ENCODERS_CUSTOM_MALLOC`, `GS1_ENCODERS_CUSTOM_CALLOC`,
   `GS1_ENCODERS_CUSTOM_REALLOC` and `GS1_ENCODERS_CUSTOM_FREE` macros. See
   below for implementation details.

#### Custom heap management routines example

Define `GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H=my_alloc.h`.

Example declarations in `my_alloc.h`:

\code
#define GS1_ENCODERS_CUSTOM_MALLOC(sz) my_malloc(sz)
#define GS1_ENCODERS_CUSTOM_CALLOC(nm, sz) my_calloc(nm, sz)
#define GS1_ENCODERS_CUSTOM_REALLOC(p, sz) my_realloc(p, sz)
#define GS1_ENCODERS_CUSTOM_FREE(p) my_free(p)

void* my_malloc(size_t s);
void* my_calloc(size_t nm, size_t sz);
void* my_realloc(void *p, size_t sz);
void my_free(void *p);
\endcode

Example implementation:

\code
...

void* my_malloc(size_t s) {
        void* p = malloc(s);
        printf("*** MALLOC %zu => %p\n", s, p);
        return p;
}

void* my_calloc(size_t nm, size_t sz) {
        void* p = calloc(nm, sz);
        printf("*** CALLOC %zu %zu => %p\n", nm, sz, p);
        return p;
}

void* my_realloc(void *p, size_t sz) {
        void* q = realloc(p, sz);
        printf("*** REALLOC %p %zu => %p\n", p, sz, q);
        return q;
}

void my_free(void *p) {
        printf("*** FREE %p\n", p);
        free(p);
}

...
\endcode
