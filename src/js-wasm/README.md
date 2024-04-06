gs1encoder
==========

JavaScript wrapper library for the GS1 Syntax Engine compiled as a WASM by
Emscripten.

This JS library is provided as an ESM and therefore requires a modern Node.js engine.


Documentation
-------------

The library is a lightweight wrapper around the Syntax Engine native C library described here:
<https://gs1.github.io/gs1-syntax-engine/>


Example
-------

A minimal example is as follows:

```shell
npm init es6
...
npm install --save gs1encoder
```

`example.js`:

```javascript
import { GS1encoder } from "gs1encoder";

var gs1encoder = new GS1encoder();
await gs1encoder.init();

console.log("\nSyntax Engine version: %s\n", gs1encoder.version);

gs1encoder.aiDataStr = "(01)09521234543213(99)TESTING123";

console.log("AI element string:              %s", gs1encoder.aiDataStr);
console.log("Barcode message:                %s", gs1encoder.dataStr);
console.log("Canonical GS1 Digital Link URI: %s", gs1encoder.getDLuri(null));

gs1encoder.free();
```

```shell
$ node example.js

Syntax Engine version: Apr  6 2024

AI element string:              (01)09521234543213(99)TESTING123
Barcode message:                ^010952123454321399TESTING123
Canonical GS1 Digital Link URI: https://id.gs1.org/01/09521234543213?99=TESTING123
```

An comprehensive, interactive example is provided here:
<https://github.com/gs1/gs1-syntax-engine/blob/main/src/js-wasm/example.mjs>
