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

Build and publish the npm package:

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
