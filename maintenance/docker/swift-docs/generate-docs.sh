#!/bin/bash

set -euo pipefail

# Output directory (we're in /srv/src/swift, docs is at /srv/docs)
OUTPUT_DIR="/srv/docs/swift"

# Clean previous documentation
rm -rf "$OUTPUT_DIR"

# Generate documentation using Jazzy
# This will build the full package (including C library) but only document the Swift wrapper
jazzy \
    --module GS1Encoders \
    --author "GS1 AISBL" \
    --author_url "https://www.gs1.org/" \
    --github_url "https://github.com/gs1/gs1-syntax-engine" \
    --root-url "https://gs1.github.io/gs1-syntax-engine/swift/" \
    --output "$OUTPUT_DIR" \
    --readme README.md \
    --clean \
    --skip-undocumented \
    --hide-documentation-coverage

# Remove docsets directory containing binary assets
rm -rf "$OUTPUT_DIR/docsets"
