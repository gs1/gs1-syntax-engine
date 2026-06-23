// swift-tools-version: 5.9

/*
 * Copyright (c) 2022-2026 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import PackageDescription

let package = Package(
    name: "GS1Encoders",
    platforms: [
        .iOS(.v13),
        .macOS(.v10_15),
        .watchOS(.v6),
        .tvOS(.v13),
        .visionOS(.v1),
    ],
    products: [
        .library(name: "GS1Encoders", targets: ["GS1Encoders"]),
    ],
    targets: [
        .target(
            name: "CGS1Encoders",
            exclude: ["c-lib/aitable.inc"],
            cSettings: [
                .define("PRNT", to: "0"),
                .define("GS1_LINTER_ERR_STR_EN"),
                .headerSearchPath("include"),
                .headerSearchPath("c-lib"),
                .headerSearchPath("c-lib/syntax"),
            ]
        ),
        .target(name: "GS1Encoders", dependencies: ["CGS1Encoders"]),
        .testTarget(
            name: "GS1EncodersTests",
            dependencies: ["GS1Encoders"],
            resources: [.copy("gs1-syntax-dictionary.txt")]
        ),
    ]
)
