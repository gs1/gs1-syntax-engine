#!/usr/bin/env bash
#
# Regenerate the gs1encoders-swift distribution package from src/swift (+ vendored c-lib).
# Usage: maintenance/sync-swift-package.sh <path-to-gs1encoders-swift>
#
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

DIST="${1:?usage: maintenance/sync-swift-package.sh <path-to-gs1encoders-swift>}"
DIST="$(cd "$DIST" && pwd)"

VERSION="$(grep -m1 '"version"' src/js-wasm/package.json | sed -E 's/.*"version"[[:space:]]*:[[:space:]]*"([^"]+)".*/\1/')"
SRC_SHA="$(git rev-parse HEAD)"

echo "Syncing GS1Encoders Swift package v${VERSION} (source gs1-syntax-engine@${SRC_SHA:0:12}) -> ${DIST}"

# 1. Wipe generated content (idempotent — deletions propagate); preserve .git.
find "$DIST" -mindepth 1 -maxdepth 1 ! -name .git -exec rm -rf {} +

# 2. The Swift sources and tests (the demo executable is omitted from the distribution).
cp -RP src/swift/Sources "$DIST/Sources"
rm -rf "$DIST/Sources/Example"
cp -RP src/swift/Tests "$DIST/Tests"
if [ -f LICENSE ]; then cp LICENSE "$DIST/LICENSE"; else echo "WARNING: no LICENSE at repo root"; fi
printf '.build/\n.swiftpm/\n' >"$DIST/.gitignore"

# Generated files from templates (curated library-only manifest, README, CONTRIBUTING, SPI, CI).
cp maintenance/swift-dist-templates/Package.swift.template "$DIST/Package.swift"
sed "s/@VERSION@/${VERSION}/g" maintenance/swift-dist-templates/README.md >"$DIST/README.md"
cp maintenance/swift-dist-templates/CONTRIBUTING.md "$DIST/CONTRIBUTING.md"
cp maintenance/swift-dist-templates/spi.yml "$DIST/.spi.yml"
mkdir -p "$DIST/.github/workflows"
cp maintenance/swift-dist-templates/ci.yml "$DIST/.github/workflows/ci.yml"

# Vendor the C library (git archive: tracked sources only, no build/ artifacts).
rm -f "$DIST/Sources/CGS1Encoders/c-lib"
mkdir -p "$DIST/Sources/CGS1Encoders/c-lib"
git archive HEAD src/c-lib | tar -x --strip-components=2 -C "$DIST/Sources/CGS1Encoders/c-lib"

# Keep only sources the Swift target builds; aitable.inc stays (it is #included).
(cd "$DIST/Sources/CGS1Encoders/c-lib" &&
	rm -f ./*.vcxproj ./*.vcxproj.filters ./*.cpp ./*.hpp ./*.pl Makefile README.md \
		gs1-syntax-dictionary.txt example.c gs1encoders-test.c gs1encoders-fuzzer-*.c acutest.h &&
	rm -f syntax/gs1syntaxdictionary-test.c syntax/acutest.h syntax/unittest.h syntax/test-gcp-lookup.h)

# Replace the umbrella-header symlink with a real copy so the module map resolves it.
rm -f "$DIST/Sources/CGS1Encoders/include/gs1encoders.h"
cp "$DIST/Sources/CGS1Encoders/c-lib/gs1encoders.h" "$DIST/Sources/CGS1Encoders/include/gs1encoders.h"

# Fail if any symlink remains (it would escape the package or be dropped).
if find "$DIST/Sources" "$DIST/Tests" -type l | grep -q .; then
	echo "ERROR: unresolved symlinks remain in the generated package:" >&2
	find "$DIST/Sources" "$DIST/Tests" -type l >&2
	exit 1
fi

# 4. Vendor the test resource so `swift test` runs standalone.
cp src/c-lib/gs1-syntax-dictionary.txt "$DIST/Tests/GS1EncodersTests/"

# 5. Provenance: record exactly what this projection was generated from.
printf '{\n  "source": "gs1-syntax-engine@%s",\n  "version": "%s"\n}\n' "$SRC_SHA" "$VERSION" >"$DIST/.gs1-sync.json"

echo "Generated. Verify with: swift build && swift test (in ${DIST})"
