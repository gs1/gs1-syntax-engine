/*
 *  Basic node.js example for using the JavaScript wrapper around the WASM
 *  build of the GS1 Syntax Engine.
 *
 *  Copyright (c) 2022 GS1 AISBL.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  The API reference for the native C library is available here:
 *
 *  https://gs1.github.io/gs1-syntax-engine/
 *
 */

"use strict";


/*
 * Temporary boilerplate required due to deficiencies in Emscripten's modulular
 * JS wrapper. May be able to drop this in due course.
 *
 */
globalThis.__dirname = '.';
await import('module').then(module => globalThis.require = module.createRequire(import.meta.url));


import { GS1encoder } from "./gs1encoder.mjs";

var gs1encoder = new GS1encoder();
await gs1encoder.init();

var input = 'https://example.com/01/1231231231232/10/ABC123?99=TEST';
console.log("Given GS1 DL URI:  " + input);

gs1encoder.dataStr = input;
console.log("AI element string: " + gs1encoder.aiDataStr);

console.log("HRI:");
gs1encoder.hri.forEach(ai => console.log("       " + ai));

gs1encoder.free();
