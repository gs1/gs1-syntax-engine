# Maintenance Notes

These notes are intended for project maintainers, not regular users of the library.

## Release Procedure

Sync with latest Syntax Dictionary release:

- `make -C src/c-lib syncsyntaxdict`
- Ensure that a version is tagged in the gs1-syntax-dictionary.txt file.

Sanity checks:

```bash
make -C src/c-lib clean-test
make -C src/c-lib test SANITIZE=yes
make -C src/c-lib fuzzer
# Run fuzzers.
```

Freshen copyright notices:

```bash
make -C src/c-lib copyright
```

Update CHANGES file, ensuring that the correct project version is set:

```bash
editor CHANGES
# Finalise entry for new version

VERSION=$(egrep -m1 '^[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$' CHANGES)
echo "$VERSION"
```

Tag a release and push:

```bash
git tag "$VERSION"
git push origin "$VERSION"
git push gs1 "$VERSION"
```

Manually update the release notes.

Align Node.js and Java package versions with project version.

```bash
make -C src/c-lib setversion
```

## Build and publish the npm package

```bash
make -C src/c-lib docs-js-wasm       # Regenerate .d.ts
make -C src/c-lib clean-wasm
docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk make -C src/c-lib wasm
cd src/js-wasm
npm install
npm test
npm pack
npm login
npm publish
```

## Build and publish the Swift package

The Swift binding is packaged into the separate `gs1/gs1encoders-swift`
repository:

```bash
# Clone the distribution repo first if you do not have it:
#   git clone git@github.com:gs1/gs1encoders-swift.git ../gs1encoders-swift
make -C src/c-lib swift-dist SWIFT_DIST=../gs1encoders-swift
```

Verify it builds and tests as a standalone package:

```bash
docker run --rm -v $(pwd)/../gs1encoders-swift:/pkg -w /pkg -e HOME=/tmp -u $(id -u):$(id -g) \
    swift:6.0 bash -lc 'swift build && swift test'
```

Publish as a single commit tagged with the project version:

```bash
cd ../gs1encoders-swift
git checkout --orphan release && git add -A
git commit -m "GS1 Barcode Syntax Engine Swift package <version>"
git branch -M release main
git tag -f <version>
git push --force origin main && git push --force origin <version>
```

