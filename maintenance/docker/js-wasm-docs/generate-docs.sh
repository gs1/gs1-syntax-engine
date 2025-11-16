#!/bin/bash

set -euo pipefail

# Generate HTML documentation from JSDoc
echo "Generating JSDoc HTML documentation..."
jsdoc -c jsdoc.json

# Generate TypeScript declarations from JSDoc
echo "Generating TypeScript declarations..."
tsc --allowJs \
    --declaration \
    --emitDeclarationOnly \
    --skipLibCheck \
    --moduleResolution node \
    --module ES2020 \
    --target ES2020 \
    --lib ES2020 \
    --outDir ../../../src/js-wasm \
    ../../../src/js-wasm/gs1encoder.mjs

# Rename .d.mts to .d.ts for npm compatibility
mv ../../../src/js-wasm/gs1encoder.d.mts ../../../src/js-wasm/gs1encoder.d.ts

# Remove internal WASM wrapper types - users should only use gs1encoder.mjs
rm -f ../../../src/js-wasm/gs1encoder-wasm.d.mts

echo "Documentation generation complete!"
