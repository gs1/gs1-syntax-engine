GS1 Syntax Engine
=================

The GS1 Syntax Engine is a library that supports the processing of
GS1 syntax data, including Application Identifier element strings and GS1
Digital Link URIs. It includes a native C library together with bindings
for C# .NET, and is intended to be integrated into the widest variety of
settings.

The project also serves as a reference implementation of a framework for
processing the [GS1 Syntax Dictionary and
Linters](https://github.com/gs1/gs1-syntax-dictionary).

This project includes:

  * A C library that can be:
    * Vendored into third-party code.
    * Compiled to native code for use as a shared library (Linux / MacOS / BSD) or dynamic-link library (Windows).
    * Compiled to WebAssembly (or JavaScript) for use in a browser-based web application or Node.js application.
  * A C# .NET wrapper class that provides an object interface to the native library from managed code, using Platform Invoke (P/Invoke).
  * Several example applications:
    * A demonstration console application whose code shows how to use the native library.
    * A demonstration desktop application using Windows Presentation Foundation (WPF) whose code shows how to use the C# .NET wrapper to access the native library.
    * A demonstration whose code shows how to use the WebAssembly (or JavaScript) build from a browser-based web application.
    * A demonstration whose code shows how to use the WebAssembly (or JavaScript) build from a Node.js console application.


Documentation
-------------

The C library API is fully documented in the docs/ directory and is
available online here: <https://gs1.github.io/gs1-syntax-engine/>

Instructions for getting started with the console application are provided in
the [Console Application User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Console-Application-User-Guide).

Instructions for getting started with the desktop application are provided in
the [Desktop Application User Guide](https://github.com/gs1/gs1-syntax-engine/wiki/Desktop-Application-User-Guide).


Using the library
------------------

The library is provided with full source and also in the form of a pre-built
library (portable DLL) along with associated development headers (.h) and
linker (.lib) files.

Pre-built assets are available here:

<https://github.com/gs1/gs1-syntax-engine/releases/latest>

The license is permissive allowing for the source code to be vendored into an
application codebase (Open Source or proprietary) or for the pre-built shared
library to be redistributed with an application.

This repository contains:

| Directory      | Purpose                                                                                                                                    |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| src/c-lib      | Source for the native C library ("The library"), unit tests, fuzzers and demo console application                                          |
| docs           | Documentation for the public API of the native C library                                                                                   |
| src/dotnet-lib | C# .NET wrappers that provide a managed code interface to the native library using P/Invoke                                                |
| src/dotnet-app | A demo C# .NET desktop application (WPF) that uses the wrappers and native library                                                         |
| src/js-wasm    | A JavaScript wrapper that provides an developer-friendly interface to the WASM or JavaScript build, with demo web and console applications |


### Building on Windows

The library, wrappers, demonstration console application and demonstration
desktop application can be rebuilt on Windows using MSVC.

The project contains a solution file (.sln) compatible with recent versions of
Microsoft Visual Studio. In the Visual Studio Installer you will need to ensure
that MSVC is installed by selecting the "C++ workload" and that a recent .NET
Core SDK is available. You must build using the "`x86`" solution platform.

Alternatively, all components can be built from the command line by opening a
Developer Command Prompt, cloning this repository, changing to the `src`
directory and building the solution using:

    msbuild /p:Configuration=release gs1encoders.sln


### Building on Linux or MacOS

The library and demonstration console application can be rebuilt on any Linux
or macOS system that has a C compiler (such as GCC or Clang).

To build using the default compiler change into the `src/c-lib` directory and run:

    make

A specific compiler can be chosen by setting the CC argument for example:

    make CC=gcc

    make CC=clang

There are a number of other targets that are useful for library development
purposes:

    make test [SANITIZE=yes]  # Run the unit test suite, optionally building using LLVM sanitizers.
    make fuzzer               # Build fuzzers for exercising the individual encoders. Requires LLVM libfuzzer.

The WASM build artifacts can be generated by first installing and activating
the Emscripten SDK and then running:

    make wasm [JSONLY=yes]    # Use argument JSONLY=yes to create a JS-only build that does not use WebAssembly.


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


Installing the Pre-built Demo Web Browser Application
-----------------------------------------------------

A demonstration web browser application is provided in the form of a
WebAssembly executable (with supporting JavaScript loader), a JavaScript
wrapper (providing the user API) and HTML + JS implementation files. Compatible
with all modern web browsers.

The most recent version of the web application can be
[downloaded from here](https://github.com/gs1/gs1-syntax-engine/releases/latest).

Download the asset named `gs1encoders-wasm-app.zip`, extract it and place the
resulting files in a single directory to served by a web server.

Ensure that the `.wasm` file is served with the MIME type `application/wasm`.

Simply point a WebAssembly-enabled browser at the HTTP location of the `.html`
file and the web application will load.


License
-------

Copyright (c) 2000-2022 GS1 AISBL

Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this library except in compliance with the License.

You may obtain a copy of the License at:

<http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
