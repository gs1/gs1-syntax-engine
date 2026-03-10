# gs1encoder

JavaScript wrapper library for the GS1 Barcode Syntax Engine compiled as a Wasm by
Emscripten.

The GS1 Barcode Syntax Engine provides routines that support the processing of
GS1 syntax data, including Application Identifier element strings and GS1
Digital Link URIs, whether these are provided in raw or human-friendly format
or as normalised scan data received from barcode readers.

## Documentation

Full API documentation with usage examples is available at:
<https://gs1.github.io/gs1-syntax-engine/js-wasm/>

## Installation

```shell
npm init es6
npm install gs1encoder
```

## Quick Start

```javascript
import { GS1encoder } from "gs1encoder";

const gs = await GS1encoder.create();

gs.aiDataStr = "(01)09521234543213(99)TESTING123";
console.log("GS1 Digital Link URI: " + gs.getDLuri("https://example.com"));

gs.free();  // Release native resources when done
```

**Note:** Each `GS1encoder` instance allocates native resources. Call `free()` when you
are finished with an instance to release these resources promptly.

## Examples

- Node.js: [example.node.mjs](https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.node.mjs)
- Browser: [example.html](https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.html) with [example.mjs](https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.mjs)

## TypeScript Support

This library includes type definitions (`.d.ts` file) that provide compile-time
type checking in TypeScript projects and autocompletion in many IDEs.
