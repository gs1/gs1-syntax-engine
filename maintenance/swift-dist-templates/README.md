<!-- Generated from https://github.com/gs1/gs1-syntax-engine (src/swift); do not edit here. -->
# GS1 Barcode Syntax Engine — Swift Package

Swift wrapper library for the GS1 Barcode Syntax Engine.

The GS1 Barcode Syntax Engine provides routines that support the processing of GS1 syntax
data, including Application Identifier element strings and GS1 Digital Link URIs, whether
these are provided in raw or human-friendly format or as normalised scan data received
from barcode readers.

The implementations are intended for use with GS1 standards and applications and do not
contain additional features that might be required for more general use.

The native engine is compiled from source, so the package runs anywhere Swift does — iOS,
macOS, tvOS, watchOS, visionOS, and Linux. This repository is a generated distribution of
the Swift binding maintained in
[gs1/gs1-syntax-engine](https://github.com/gs1/gs1-syntax-engine) (`src/swift`); please
file issues and contribute there.

## Documentation

Full API documentation with usage examples is available at:
<https://gs1.github.io/gs1-syntax-engine/swift/>

## Installation

Add the package to your `Package.swift`:

```swift
dependencies: [
    .package(url: "https://github.com/gs1/gs1encoders-swift", from: "@VERSION@"),
],
targets: [
    .target(name: "MyApp", dependencies: [
        .product(name: "GS1Encoders", package: "gs1encoders-swift"),
    ]),
]
```

In Xcode, choose **File → Add Package Dependencies…** and enter
`https://github.com/gs1/gs1encoders-swift`.

## Quick Start

```swift
import GS1Encoders

do {
    let gs = try GS1Encoder()
    try gs.setAIdataStr("(01)09521234543213(99)TESTING123")
    print("GS1 Digital Link URI: \(try gs.getDLuri("https://example.com"))")
} catch {
    print("Error: \(error)")
}
```

**Note:** Each `GS1Encoder` instance allocates native resources that are released
automatically when the instance is deallocated. You may also call `free()` explicitly to
release them before the instance goes out of scope.

**Note:** The library is thread-safe provided that each thread operates on its own
`GS1Encoder` instance. Do not share a single instance across threads.

## Examples

- Console example: [Example.swift](https://github.com/gs1/gs1-syntax-engine/blob/main/src/swift/Sources/Example/Example.swift)
- iOS app with barcode scanning and a full UI: [GS1 Encoders iOS App](https://github.com/gs1/gs1-syntax-engine/tree/main/src/ios)

## License

Apache-2.0. See [LICENSE](LICENSE).
