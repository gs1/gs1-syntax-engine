GS1 Barcode Syntax Engine
=========================

The GS1 Barcode Syntax Engine ("Syntax Engine") is a library that supports the processing of
GS1 syntax data, including Application Identifier element strings and GS1
Digital Link URIs. It includes a native C library together with bindings
for C# .NET, Java and Swift, that is intended to be integrated into a wide
variety of platforms.

The project also serves as a reference implementation of a framework for
processing the [GS1 Barcode Syntax Dictionary](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/syntax-dictionary/)
and its subordinate [GS1 Barcode Syntax Tests](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/syntax-tests/).

This project includes:

  * A C library that can be:
    * Vendored into third-party code.
    * Compiled to native code for use as a shared library (Linux / MacOS / BSD) or dynamic-link library (Windows).
    * Compiled to WebAssembly or pure JavaScript for use in a browser-based web application or Node.js application.
  * A C# .NET wrapper class that provides an object interface to the native library from managed code, using Platform Invoke (P/Invoke).
  * A Java wrapper class that provides an object interface to the native library from managed code, using Java Native Interface.
  * A Swift module that provides a Swift-friendly wrapper around a native C library using Swift’s C interoperability.
  * Several example applications:
    * A console application whose C code shows how to use the native library.
    * A console application whose Java code shows how to use the Java Native Interface wrapper.
    * A console application whose Swift code shows how to use the Swift wrapper.
    * A desktop application using Windows Presentation Foundation (WPF) whose code shows how to use the C# .NET wrapper.
    * A browser-based web application that shows how to use the WebAssembly or pure JavaScript build of the library.
    * A Node.js console application that shows how to use the WebAssembly or pure JavaScript build of the library.
    * An example Node.js web service that provides HTTP access to the library's functionality, together with an example HTTP client application.
    * An Android Studio project that shows how to use the Java wrapper from Kotlin to create an Android app that includes support for decoding GS1 data in barcodes scanned using ML Kit.
    * An Xcode project that shows how to use the Swift wrapper to create an iOS app that includes support for decoding GS1 data in barcodes scanned using ML Kit.

The above make the GS1 Barcode Syntax Resource suitable for a large number of [deployment scenarios](https://github.com/gs1/gs1-syntax-engine/wiki/Deployment-Scenarios).


Repository layout
-----------------

This repository layout is as follows:

| Directory      | Purpose                                                                                                                                         |
| -------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| src/c-lib      | Source for the native C library ("The library"), unit tests, fuzzers and demo console application                                               |
| docs           | Documentation for the public API of the native C library                                                                                        |
| src/dotnet-lib | C# .NET wrappers that provide a managed code interface to the native library using P/Invoke                                                     |
| src/dotnet-app | A demo C# .NET desktop application (WPF) that uses the wrappers and native library                                                              |
| src/js-wasm    | A JavaScript wrapper that provides an developer-friendly interface to the WASM or pure JavaScript build, with demo web and console applications |
| src/java       | A Java wrapper that provides a managed code interface to the native library using Java Native interface                                         |
| src/swift      | A Swift module that provides a Swift-friendly wrapper around a native library using Swift’s C interoperability.                                 |
| src/android    | An Android Studio project that demonstrates how to use the Java wrapper from Kotlin to create an Android app                                    |
| src/ios        | An Xcode project that demonstrates how to use the Swift wrapper to create an iOS app                                                            |


Documentation
-------------

API documentation and Quick Start guides are available for each supported language binding:

| Language/binding/platform  | Documentation link                                 |
| -------------------------- | -------------------------------------------------- |
| Native C libary            | <https://gs1.github.io/gs1-syntax-engine/>         |
| C# .NET binding            | <https://gs1.github.io/gs1-syntax-engine/cs/>      |
| Java binding (and Android) | <https://gs1.github.io/gs1-syntax-engine/java/>    |
| Swift binding (and iOS)    | <https://gs1.github.io/gs1-syntax-engine/swift/>   |
| JS + WebAssembly build     | <https://gs1.github.io/gs1-syntax-engine/js-wasm/> |

Instructions for getting started with the console application are provided in
the [Console Application User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide).

Instructions for getting started with the desktop application are provided in
the [Desktop Application User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Desktop-Application-User-Guide).


Prebuilt assets
---------------

The library is provided with full source and also in the form of a pre-built
library (portable DLL) along with associated development headers (.h) and
linker (.lib) files.

Pre-built assets are available here:

<https://github.com/gs1/gs1-syntax-engine/releases/latest>

The license is permissive allowing for the source code to be vendored into an
application codebase (Open Source or proprietary) or for the pre-built shared
library to be redistributed with an application.


Installing the Pre-built Demo Console Application
-------------------------------------------------

A demonstration console application is provided in the form of an .EXE file
compatible with modern 64-bit Windows operating systems and as a .bin file
compatible with 64-bit Linux operating systems. There are no installation
dependencies and the file can be run from any location on the file system.

The most recent version of the console application can be
[downloaded from here](https://github.com/gs1/gs1-syntax-engine/releases/latest).

For Windows systems download the asset named
`gs1encoders-windows-console-app.zip`. For Linux systems download the asset
named `gs1encoders-linux-app.zip`. In the event of issues with antivirus software
consult the note in the
[User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide).

The pre-built application requires that the Visual C++ Redistributable 2019 (32
bit) is installed: <https://visualstudio.microsoft.com/downloads/#microsoft-visual-c-redistributable-for-visual-studio-2019>


Installing the Pre-built Demo Desktop Application
-------------------------------------------------

A demonstration desktop application is provided in the form of an .EXE file
compatible with modern 64-bit Windows operating systems and a recent .NET
Framework.

The most recent version of the desktop application can be
[downloaded from here](https://github.com/gs1/gs1-syntax-engine/releases/latest).

For Windows systems download the asset named `gs1encoders-windows-gui-app.zip`. In
the event of issues with antivirus software consult the note in the
[User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Desktop-Application-User-Guide).

The pre-built application requires that the .NET Core 3.1 Desktop Runtime -
Windows x86 is installed: <https://dotnet.microsoft.com/download/dotnet/3.1/runtime>


Installing the Pre-built Demo Web Browser Application and Node.js Application
-----------------------------------------------------------------------------

A demonstration build, that can be run as either a web browser application or a
Node.js console application, is provided in two flavours:

1. A compilation to a WebAssembly executable with supporting JavaScript loader
2. A transpilation to pure JavaScript ("asm.js") with associated mem file

Each of these flavours includes the JavaScript wrapper (providing the user API)
and HTML / JS / Node.js implementation files. They are compatible with all modern
web browsers and any maintained release of Node.js.

The most recent version can be
[downloaded from here](https://github.com/gs1/gs1-syntax-engine/releases/latest).

Download the asset named `gs1encoders-wasm-app.zip` or `gs1encoders-jsonly-app.zip`
based on the required flavour (WASM or pure JavaScript, respectively).

To use the demo web application, extract the ZIP file and place the resulting files
in a single directory to be served by a web server as static content. Simply point a
WebAssembly-enabled browser at the HTTP location of the `.html` file and the web
application will load.

Note: For the WASM build, ensure that the web server is configured to serve the
`.wasm` file with the MIME type `application/wasm`.

To use the demo Node.js console application, extract the ZIP file into a single
directory and start it by running the following from within the same directory:

    node example.node.mjs

