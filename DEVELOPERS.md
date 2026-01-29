# GS1 Barcode Syntax Engine

This document is for developers contributing to the library, not users of the
library. It is authoritative for both human contributors and automated tools.


## Background

The GS1 Barcode Syntax Engine is a native C library with bindings for C#/.NET,
Java (JNI), Swift, and JavaScript/WebAssembly.

It is part of the [GS1 Barcode Syntax Resource](https://www.gs1.org/standards/gs1-barcodes/gs1-barcode-syntax-resource),
a suite of tools for implementing GS1 standards that also includes the
[GS1 Barcode Syntax Dictionary](https://github.com/gs1/gs1-syntax-dictionary).

It processes GS1 syntax data including:

- Application Identifier (AI) element strings (bracketed and unbracketed)
- GS1 Digital Link URIs
- Barcode scan data from various GS1 symbologies

The project is a reference implementation of a framework for processing the GS1
Barcode Syntax Dictionary and its associated Linters.


### Contribution Rules and Constraints

- Create atomic, logical commits that complete one task
- Match existing code style and idioms
- Ensure unit tests cover new functionality and error conditions
- Document code that works around compiler bugs or satiates static analysers with a comment explaining why
- Don't create breaking changes affecting the public API, i.e. don't force code changes upon users of the library or require re-linking. Use `GS1_ENCODERS_DEPRECATED` macro for functions being phased out
- Avoid deeply nested code by exiting blocks early with `return`, `continue`, `break`, etc.
- Regenerate documentation, including API references and examples, whenever there are changes.

### Generated and Vendored-in Files

These files are automatically generated or synced from external sources - do not edit directly:

| Path                    | Source                        | Regenerate with       |
|-------------------------|-------------------------------|-----------------------|
| `src/c-lib/syntax/*`    | GS1 Syntax Dictionary project | `make syncsyntaxdict` |
| `src/c-lib/aitable.inc` | gs1-syntax-dictionary.txt     | `make syncsyntaxdict` |


## LLM Rules

- Do not push commits or rewrite history
- If a file is updated then its copyright date should be bumped to the current year
- Backup work before running irreversible commands
- Plan complex tasks; interview the user regarding significant design choices
- Always search for and follow pre-existing patterns before writing code


## C Programming Goals

The C library is suitable for embedded, desktop, and server environments. Code
should be portable and avoid platform-specific dependencies.

As a reference implementation, the code should be efficient yet instructive.
Avoid opinionated abstractions that enforce a particular way of performing
standard GS1 processes - the library demonstrates how to process GS1 data, not
how users must structure their applications.

### Coding Standards

- C code must compile on MSVC, GCC and Clang (including Apple's Clang variant)
- All C code must compile cleanly with `-Wall -Wextra -Wconversion -Werror -pedantic`
- Use `const` liberally - both for pointer targets and the pointers themselves: `const char* const str`
- Use Doxygen-style comments for public API functions (`@param`, `@return`, `@note`)
- Use `size_t` for iterating unbounded memory, otherwise native-width `int` if sufficient; avoid smaller types that may actually reduce performance
- Use `DIAG_{PUSH,POP,DISABLE_*}` macros to suppress warnings in specific code sections while maintaining `-Werror` builds

### Error Handling

**Error state pattern:**
- Public API functions begin with `reset_error(ctx)` to clear previous error state
- Setter functions return `bool` (true=success, false=failure)
- Error details stored in context, retrieved via `gs1_encoder_getErrMsg(ctx)`
- Use `SET_ERR()` and `SET_ERR_V()` macros for consistent error reporting with optional format arguments

**Appropriate use of goto is encouraged:**
- Jumping to a common cleanup/exit block (e.g., `goto fail`, `goto out`)
- Retry logic with backward jumps (e.g., `goto again`, `goto redo`)
- Skipping forward to avoid a long nested block

**Assertions for preconditions:**
- Use `assert()` for internal function contracts and invariants
- Document assumptions about parameters (e.g., valid pointers, length constraints)
- Assertions are for programmer errors, not user input validation
- Assertions are extremely valuable for fuzzing - they catch invariant violations that might otherwise cause silent corruption or go unnoticed

### Memory Allocation

**Context buffers**: Most working buffers are pre-allocated within the context
struct, avoiding per-call heap allocation. Prefer using context buffers over
allocating new memory.

**Heap allocation**:
- Don't heap allocate after init is complete.
- Always use the `GS1_ENCODERS_*` macros, never direct `malloc`/`free`, to enable users to use their preferred heap management framework. See the [C API documentation](https://gs1.github.io/gs1-syntax-engine/) for a custom heap management example.

**Stack allocation** (`alloca`):
- Use only when necessary to avoid large extensions to the context structure or to avoid runtime heap allocation
- Hoist out of loops to avoid repeated stack growth

### Performance Patterns

The codebase prioritises performance and minimal allocations. Follow these
patterns:

**Avoid redundant work and temporary buffers:**
- Write directly to output rather than intermediate buffers
- For internal purposes, pass position and length rather than building C strings
- Use in-place tokenization (`gs1_tokenise` utility) to avoid allocations during parsing
- Track output position instead of using `strcat`/`strncat`
- Don't walk strings multiple times (e.g., `strspn` then `strlen`)
- Cache and pass string lengths instead of repeated `strlen` calls
- Hoist invariants like `strlen` out of loops
- Internal functions should accept pointer and length rather than NUL-terminated strings - the caller typically already knows the length

**Efficient lookups and avoiding expensive functions:**
- Use lookup tables for character validation instead of walking lists
- Use binary search for sorted data (see `gs1_bsearch` utility)
- Use bitfields to track sets efficiently (e.g., emitted AIs)
- Precompute repeatedly accessed data at compile time or load time (e.g., `ailen` field)
- No `sprintf`/`snprintf` - use `memcpy` with tracked output position
- No `strspn`/`strchr` for single characters - use direct comparisons
- Avoid `atof`/`strtoul` where a direct implementation is more efficient (for simple cases)
- Prefer `memcpy` over `strcpy`/`strncpy` when length is already known

**Compiler hints:**
- Mark functions as `pure` or `const` where applicable
- Use `likely`/`unlikely` to hint that error paths are unlikely

### Naming Conventions

Use prefixes to avoid collisions with user code:

| Scope                | Prefix          | Example                     |
|----------------------|-----------------|-----------------------------|
| Public API functions | `gs1_encoder_`  | `gs1_encoder_init()`        |
| Public types         | `gs1_encoder_`  | `gs1_encoder_symbologies_t` |
| Public macros        | `GS1_ENCODERS_` | `GS1_ENCODERS_API`          |
| Internal functions   | `gs1_`          | `gs1_parseAIdata()`         |


## Directory Structure

### Source Code

| Directory            | Purpose                                                           |
|----------------------|-------------------------------------------------------------------|
| `src/c-lib/`         | Native C library, unit tests, fuzzers, console demo application   |
| `src/c-lib/syntax/`  | Linter routines (synced from gs1-syntax-dictionary project)       |
| `src/dotnet-lib/`    | C# .NET P/Invoke wrapper                                          |
| `src/dotnet-app/`    | Demo C# .NET desktop application (WPF)                            |
| `src/java/`          | Java JNI wrapper and demo console application                     |
| `src/swift/`         | Swift package with demo console application                       |
| `src/js-wasm/`       | JavaScript wrapper with demo web and Node.js applications         |
| `src/android/`       | Android Studio project for mobile integration demo                |
| `src/ios/`           | Xcode project for iOS integration demo                            |
| `src/contrib/`       | Community-contributed bindings (Rust, Python) - proof of concept  |

### Key C Library Files

| File                         | Purpose                                              |
|------------------------------|------------------------------------------------------|
| `gs1encoders.h`              | Public API header with all exported functions        |
| `enc-private.h`              | Private header with internal definitions             |
| `gs1encoders.c`              | API implementation and context management            |
| `ai.c`                       | Application Identifier processing and validation     |
| `dl.c`                       | GS1 Digital Link URI processing                      |
| `scandata.c`                 | Barcode scan data parsing for various symbologies    |
| `syn.c`                      | Syntax Dictionary file parsing                       |
| `gs1-syntax-dictionary.txt`  | Vendored copy of GS1 Barcode Syntax Dictionary       |
| `aitable.inc`                | Embedded AI table (generated from Syntax Dictionary) |
| `syntax/*`                   | Vendored copy of Syntax Dictionary linter routines   |
| `debug.h`                    | Debugging macros (`DEBUG_PRINT`)                     |
| `tr.h`, `tr_EN.h`            | Error message translation system                     |
| `gs1encoders-app.c`          | Console demo application source                      |
| `gs1encoders-test.c`         | Unit test harness                                    |
| `gs1encoders-fuzzer-*.c`     | Fuzzer entry points (ais, data, dl, scandata, syn)   |
| `build-embedded-ai-table.pl` | Generates `aitable.inc` from Syntax Dictionary       |


## Documentation

### User Documentation

| Location                                        | Content                                                 |
|-------------------------------------------------|---------------------------------------------------------|
| <https://github.com/gs1/gs1-syntax-engine/wiki> | User guides for demo applications, deployment scenarios |
| <https://gs1.github.io/gs1-syntax-engine/>      | Hosted API reference (C, C#, Java, JavaScript, Swift)   |

Wiki pages to update when demo application behavior changes:

- Console Application User Guide
- Desktop Application User Guide
- Example Web Service And User Application

### API Reference

The `docs/` directory contains generated API documentation:

- **C**: Doxygen (HTML)
- **C#**: DocFX
- **Java**: Javadoc
- **JavaScript**: JSDoc
- **Swift**: DocC

Rebuild with `make docs`.


## Code Structure

### Library Instance Pattern

The library uses an opaque context pattern. All state is maintained in
`gs1_encoder` instances:

```c
gs1_encoder *ctx = gs1_encoder_init(NULL);
// ... use ctx ...
gs1_encoder_free(ctx);
```

The library is thread-safe when each thread uses its own context instance. The
context contains all state and working memory for each instance of the library.

### Public vs Private Headers

| Header          | Purpose                                                  |
|-----------------|----------------------------------------------------------|
| `gs1encoders.h` | Public API - included by user application code           |
| `enc-private.h` | Private internals - included only by this library        |

The public header declares `gs1_encoder` as an opaque incomplete type. The
private header contains the complete struct definition. This separation:

- Hides implementation details from library users
- Allows internal changes without breaking the public API
- Enforces that users access state only through API functions

### Symbol Visibility

The library is compiled with `-fvisibility=hidden` so all symbols are hidden by
default. Only public API functions are exported:

- Non-static, internal functions remain hidden from the shared library unless marked with `GS1_ENCODERS_API`
- Use `static` for file-local helper functions

### Functional API for Language Bindings

The C library provides a functional interface designed to be wrapped by
object-oriented bindings:

| C library Pattern                                      | Wrapper Pattern                               |
|--------------------------------------------------------|-----------------------------------------------|
| `gs1_encoder_init()` returns opaque `ctx`              | Constructor stores `ctx` as private handle    |
| `gs1_encoder_free(ctx)`                                | Custom destructor, either GC or explicit call |
| `gs1_encoder_getX(ctx)` / `gs1_encoder_setX(ctx, val)` | Property or getter/setter methods             |
| `bool` return indicates success/failure                | Throw exception on failure                    |
| `gs1_encoder_getErrMsg(ctx)`                           | Exception message source                      |

The `ctx` pointer is a handle requiring cleanup via `gs1_encoder_free()`.
Languages handle this differently, based on development norms:

- **C#**: Destructor (`~GS1Encoder()`) calls free automatically via garbage collection
- **JavaScript**: Explicit `free()` method must be called by the user

All context initialisation occurs in `gs1_encoder_init()`, and all working
buffers are contained within the context struct.

Wrappers access all state through accessor functions
(`gs1_encoder_getX`/`gs1_encoder_setX`), never directly accessing struct
fields. This avoids dependencies on the private struct layout and allows
internal changes without breaking wrappers.

Wrappers don't manage memory - they simply copy data from the context's
buffers.

This pattern enables idiomatic APIs in each target language while the C library
remains simple and portable.

### Extensible Options Pattern

The `gs1_encoder_init_ex()` function uses a versioned options struct to avoid
ABI ossification:

```c
struct gs1_encoder_init_opts {
    size_t struct_size;  // Must be sizeof(gs1_encoder_init_opts_t)
    // ... other fields ...
};
```

- Caller sets `struct_size = sizeof(gs1_encoder_init_opts_t)` at compile time
- New fields are always added at the end of the struct
- Library compares `opts->struct_size` to detect older callers and use defaults for new fields

This allows the options struct to grow over time without breaking existing
compiled code.

### Embedded AI Table

The embedded AI table (`aitable.inc`) uses a macro-based DSL for compile-time
initialization:

```c
AI_ENTRY( "01", NO_FNC1, DL_DATA_ATTR, N,14,14,MAN,csum,gcppos2,_, ..., "GTIN" ),
```

Techniques used:

- **Token concatenation**: `cset_##c1` and `gs1_lint_##l00` convert symbolic names (e.g., `N`, `csum`) into C identifiers (`cset_N`, `gs1_lint_csum`)
- **Placeholder macros**: `__` expands to null values for unused component slots
- **Self-documenting constants**: `DO_FNC1`, `NO_FNC1`, `MAN`, `OPT` make entries readable
- **Compile-time length**: `sizeof(ai) - 1` calculates AI string length without runtime cost
- **Static initialization**: The entire table is initialized at compile time with zero runtime parsing

This provides a human-readable format that is type-checked by the compiler and has no runtime overhead.

### Error Message Translations

Translations use conditional include files selected at compile time:

- Default language: `EN` (override with `-DGS1_ENCODERS_ERR_LANG=XX`)
- Macro expansion generates `#include "tr_EN.h"` (or other language file)
- `SET_ERR(x)` and `SET_ERR_V(x, ...)` macros look up translated strings via `ERR_TR(x)`

### Internal Helper Functions

Reusable utilities in `enc-private.h`:

| Function           | Purpose                                                    |
|--------------------|------------------------------------------------------------|
| `gs1_binarySearch` | Binary search with compare and optional validate callbacks |
| `gs1_tokenise`     | In-place tokenization without allocation                   |
| `gs1_allDigits`    | Check if buffer contains only digits                       |

**Binary search with validation**: `gs1_binarySearch` takes two function pointers:

- `compare(key, element, index)`  - standard comparison for search
- `validate(key, element, index)` - optional post-match validation

Returns:

- `>= 0`                 - index, if found and valid
- `GS1_SEARCH_NOT_FOUND` - no match
- `GS1_SEARCH_INVALID`   - match found, but validation failed

This pattern enables prefix-based lookups with additional constraints. For
example, AI lookups find a prefix match, then validate that the AI length is
correct and doesn't conflict with known AIs.

The distinct return values allow callers to differentiate "unknown AI" from
"invalid AI".

### Debugging

Use `DEBUG_PRINT()` macro for diagnostic output during development:

- Enabled by compiling with `DEBUG=yes` (e.g., `make DEBUG=yes`)
- Compiles to empty statement when disabled - no runtime overhead
- Useful for tracing AI parsing, DL URI processing, and validation flow

### Unit Test Pattern

Unit tests are embedded within the source files they test, not in separate test
files. This keeps tests close to the implementation and ensures they are
updated together.

Tests use the Acutest framework.

**Test function structure:**
- Test functions are defined in `.c` files, guarded by `#ifdef UNIT_TESTS`
- Declarations go in corresponding `.h` files, also guarded by `#ifdef UNIT_TESTS`
- Functions follow the naming convention `test_<module>_<description>(void)`

**General test patterns:**

```c
void test_ai_lookupAIentry(void) {
    gs1_encoder* ctx;
    TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
    TEST_CHECK(strcmp(gs1_lookupAIentry(ctx, "01", 2)->ai, "01") == 0);
    gs1_encoder_free(ctx);
}
```


## Build

### C Library (Linux/macOS)

```bash
cd src/c-lib

# Build library and static console app (default)
make -j $(nproc)

# Build all targets including shared library app
make -j $(nproc) all

# Build specific targets
make -j $(nproc) lib         # Both shared and static libraries
make -j $(nproc) libshared   # Shared library only
make -j $(nproc) libstatic   # Static library only
make -j $(nproc) app         # Console app using shared library
make -j $(nproc) app-static  # Standalone static console app
```

Note: On macOS use `sysctl -n hw.ncpu` instead of `$(nproc)`.

### C Library (Windows)

```bash
msbuild /p:Configuration=release /p:Platform="x86" /warnaserror src\gs1encoders.sln
```

### WebAssembly

```bash
# Using Emscripten Docker container
docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk make -C src/c-lib wasm

# Directly
make -C src/c-lib wasm

# Pure JavaScript (asm.js) alternative
make -C src/c-lib wasm JSONLY=yes
```

### Java

```bash
make -C src/c-lib -j $(nproc) libstatic
ant -f src/java/build.xml test
```

### npm Package (for releases)

```bash
cd src/js-wasm
npm install
npm test
npm pack
```

### iOS App

The iOS demo is a SwiftUI application that uses the Swift package from `src/swift/` and Google ML Kit for barcode scanning.

```bash
cd src/ios
pod install
xcodebuild -workspace "GS1 Encoders App.xcworkspace" -scheme "GS1 Encoders App" build CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
```

### Android App

The Android demo is a Kotlin application that integrates the C library via NDK/CMake and uses Google ML Kit with CameraX for barcode scanning.

```bash
cd src/android
./gradlew build --no-daemon
```


## Testing

### Unit Tests

```bash
cd src/c-lib

# Build and run all tests
make -j $(nproc) test

# Build and run with LLVM sanitizers (recommended)
make -j $(nproc) test SANITIZE=yes

# Build and run including slow tests
make -j $(nproc) test SLOW_TESTS=yes

# Build and run specific test suite (regex match on test name)
make test TEST="test_ai_"

# Test with embedded AI table (instead of external Syntax Dictionary)
rm gs1-syntax-dictionary.txt
make test
git checkout gs1-syntax-dictionary.txt  # Restore it
```

### Fuzzing

Coverage-guided fuzzing is provided by LLVM's LibFuzzer. Five fuzzers target
different entry points: `ais`, `data`, `dl`, `scandata`, and `syn`.

```bash
cd src/c-lib

# Build all fuzzers
make -j $(nproc) fuzzer

# Build a specific fuzzer
make -j $(nproc) fuzzer-ais
```

The build automatically seeds empty corpus directories by extracting quoted
strings from test source files and merging them via LibFuzzer's `-merge=1`
mode. This provides meaningful starting inputs derived from unit tests.

Run a fuzzer (command printed after build):

```bash
ASAN_OPTIONS="symbolize=1 detect_leaks=1" ./build-fuzzer/gs1encoders-fuzzer-ais -jobs=$(nproc) -workers=$(nproc) corpus-ais
```

To regenerate seeds for an existing corpus, delete the corpus directory first.

### JavaScript Tests

```bash
cd src/js-wasm
npm install
node --experimental-vm-modules node_modules/jest-cli/bin/jest.js
```


## Maintenance Tasks

### Sync Linters from gs1-syntax-dictionary

When the upstream gs1-syntax-dictionary project is updated:

```bash
cd src/c-lib
make syncsyntaxdict
```

This also regenerates `aitable.inc` from the Syntax Dictionary.

### Version Numbering

This project uses **Semantic Versioning** (SemVer) with MAJOR.MINOR.PATCH
format (e.g., `1.3.0`). This scheme is required to maintain parity with package
managers such as npm.

The version is defined in `CHANGES` and propagated to package manifests via
`make setversion`.

### Update Version Numbers

```bash
make -C src/c-lib setversion
```

This updates version in manifests such as `src/js-wasm/package.json` and
`src/java/build.xml` based on the version in `CHANGES`.

### Update Copyright Year

```bash
make -C src/c-lib copyright
```

### Generate Documentation

```bash
make -C src/c-lib docs           # All documentation
```


## CI/CD

GitHub Actions workflow (`.github/workflows/gs1encoders.yml`) runs:

- Linux CI with gcc and clang (with sanitizers)
- Windows CI with MSVC
- macOS CI with clang
- Static analysis (scan-build, cppcheck, IWYU)
- Java wrapper tests on Linux, macOS, Windows
- WebAssembly build and Node.js tests
- iOS and Android app builds

Releases are created automatically when version tags are pushed.

