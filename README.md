GS1 Barcode Syntax Engine
=========================

The GS1 Barcode Syntax Engine ("Syntax Engine") supports processing of GS1
syntax data, including Application Identifier element strings and GS1 Digital
Link URIs.

The project is a component of the [GS1 Barcode Syntax
Resource](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/) that serves
as a reference implementation of a framework for processing the [GS1 Barcode Syntax Dictionary](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/syntax-dictionary/)
and its subordinate [GS1 Barcode Syntax Tests](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/syntax-tests/).

### What's included

The GS1 Barcode Syntax Engine consists of a native C library together with bindings for C# .NET, Java, Swift, and JavaScript + WebAssembly, providing idiomatic interfaces to the native library from a variety of high-level programming languages.

| Language/binding   | Technology         | Platforms                | Documentation links |
| ------------------ | ------------------ | ------------------------ | ------------------- |
| C library          | Native code        | Cross-platform, embedded | [Overview](https://gs1.github.io/gs1-syntax-engine/), [Quick Start](https://gs1.github.io/gs1-syntax-engine/#autotoc_md1), [Examples](https://gs1.github.io/gs1-syntax-engine/#autotoc_md5), [API reference](https://gs1.github.io/gs1-syntax-engine/gs1encoders_8h.html) |
| C# .NET            | P/Invoke wrapper   | Windows, Linux, macOS    | [Overview](https://gs1.github.io/gs1-syntax-engine/cs/), [Quick Start](https://gs1.github.io/gs1-syntax-engine/cs/#quick-start), [Examples](https://gs1.github.io/gs1-syntax-engine/cs/#example-uses), [API reference](https://gs1.github.io/gs1-syntax-engine/cs/GS1.Encoders.GS1Encoder.html) |
| Java (and Android) | JNI wrapper        | Cross-platform, Android  | [Overview](https://gs1.github.io/gs1-syntax-engine/java/), [Quick Start](https://gs1.github.io/gs1-syntax-engine/java/org/gs1/gs1encoders/package-summary.html#quick-start-heading), [Examples](https://gs1.github.io/gs1-syntax-engine/java/org/gs1/gs1encoders/package-summary.html#example-uses-heading), [API reference](https://gs1.github.io/gs1-syntax-engine/java/org/gs1/gs1encoders/GS1Encoder.html) |
| Swift (and iOS)    | C interoperability | macOS, iOS               | [Overview](https://gs1.github.io/gs1-syntax-engine/swift/), [Quick Start](https://gs1.github.io/gs1-syntax-engine/swift/#quick-start), [Examples](https://gs1.github.io/gs1-syntax-engine/swift/#example-uses), [API reference](https://gs1.github.io/gs1-syntax-engine/swift/Classes/GS1Encoder.html) |
| JavaScript         | WebAssembly        | Browser, Node.js         | [Overview](https://gs1.github.io/gs1-syntax-engine/js-wasm/), [Quick Start](https://gs1.github.io/gs1-syntax-engine/js-wasm/#quick-start), [Examples](https://gs1.github.io/gs1-syntax-engine/js-wasm/#example-uses), [API reference](https://gs1.github.io/gs1-syntax-engine/js-wasm/GS1encoder.html) |

It is intended to be integrated into a wide
variety of [deployment scenarios](https://github.com/gs1/gs1-syntax-engine/wiki/Deployment-Scenarios).


### Example Applications

There is a large corpus of example code.

**C:**
- [Console application](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide) ([source](src/c-lib/gs1encoders-app.c))

**C# .NET:**
- [Windows WPF desktop application](https://github.com/gs1/gs1-syntax-engine/wiki/Desktop-Application-User-Guide) ([source](src/dotnet-app))

**Java:**
- Console application ([source](src/java/Example.java))
- Android mobile app (Kotlin) with ML Kit barcode scanning ([source](src/android))

**Swift:**
- Console application ([source](src/swift/Example.swift))
- iOS mobile app with ML Kit barcode scanning ([source](src/ios))

**JavaScript/WASM:**
- Browser-based web application ([source](src/js-wasm/example.html))
- Node.js console application ([source](src/js-wasm/example.node.mjs))
- Node.js HTTP web service with example client ([source](src/js-wasm/example-web-service.node.mjs))




## Getting Started with Pre-built Demos

Pre-built demonstration applications are available for quick evaluation without needing to build from source. All demos can be [downloaded from the latest release](https://github.com/gs1/gs1-syntax-engine/releases/latest).

| Application     | Filename                              | Instructions                                                                                  |
|-----------------|---------------------------------------|-----------------------------------------------------------------------------------------------|
| Windows console | `gs1encoders-windows-console-app.zip` | [Console Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide) |
| Linux console   | `gs1encoders-linux-app.zip`           | [Console Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide) |
| Windows desktop | `gs1encoders-windows-gui-app.zip`     | [Desktop Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Desktop-Application-User-Guide) |
| Browser app     | `gs1encoders-wasm-app.zip`            | Serve `example.{html+mjs}`, `gs1encoder.mjs`, `gs1encoder-wasm.{mjs+wasm}`; `.wasm` must have MIME type: `application/wasm` |
| Node.js console | `gs1encoders-wasm-app.zip`            | Run `node example.node.mjs`                                                                   |
| -               | `gs1encoders-jsonly-app.zip`          | Pure JavaScript (asm.js) alternative to WASM build; No `.wasm` file                           |


## Pre-built Library Assets

The library is also provided in the form of a pre-built library for Windows
(portable DLL), along with associated development headers (.h) and linker
(.lib) files available from the [releases page](https://github.com/gs1/gs1-syntax-engine/releases/latest).

Filename: `gs1encoders-windows-libs.zip`.

The license is permissive allowing for the source code to be vendored into an
application codebase (Open Source or proprietary) or for the pre-built shared
library to be redistributed with an application.


## Repository Layout

| Directory        | Purpose                                                                           |
| ---------------- | --------------------------------------------------------------------------------- |
| [`src/c-lib`](src/c-lib)      | Source for the native C library, unit tests, fuzzers and demo console application |
| `src/dotnet-lib` | C# .NET wrapper source                                                            |
| `src/dotnet-app` | Demo C# .NET desktop application (WPF)                                            |
| `src/java`       | Java wrapper source and demo console application                                  |
| `src/swift`      | Swift package with demo console application                                       |
| `src/js-wasm`    | JavaScript wrapper with demo web and Node.js applications                         |
| `src/android`    | Android Studio project demonstrating mobile integration                           |
| `src/ios`        | Xcode project demonstrating iOS integration                                       |
| `docs`           | Generated API documentation for all language bindings                             |
| `maintenance`    | Build scripts and tools for maintainers                                            |
