Maintenance Notes
=================

These notes are intended for project maintainers, not regular users of the library.


Release Procedure
-----------------

Sync with latest Syntax Dictionary release

  - Ensure that a version is tagged in the gs1-syntax-dictionary.txt file.
  - Freshen the embedded AI table.
    - `cd src/c-lib && cat gs1-syntax-dictionary | ./build-embedded-ai-table.pl > aitable.inc`


Sanity checks:

```
make -C src/c-lib clean-test
make -C src/c-lib test SANITIZE=yes
make -C src/c-lib fuzzer
[ Run them. ]
```


Freshen copyright notices:

```
make -C src/c-lib copyright
```


Update CHANGES file, ensursing that the correct project version is set:

```
editor CHANGES
[ Finalise entry for new version ]

VERSION=$(egrep -m1 '^[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$' CHANGES)
echo "$VERSION"
```


Tag a release and push:

```
git tag "$VERSION"
git push origin "$VERSION"
git push gs1 "$VERSION"
```

Manually update the release notes.


Align node.js and Java package versions with project version.

```
make -C src/c-lib setversion
```


Build and publish the npm package:

```
make -C src/c-lib clean-wasm
docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk make -C src/c-lib wasm
cd src/js-wasm
node --experimental-vm-modules /usr/local/bin/jest
npm pack
npm publish
```

