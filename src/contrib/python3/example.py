#
#  GS1 Barcode Syntax Engine
#
#  This is a contributed example that shows how a Python 3 language binding might
#  be developed.
#
#
#  @author Copyright (c) 2021-2026 GS1 AISBL.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

import gs1encoders
from gs1encoders import *


def main():
    gs1encoder = GS1Encoder()

    print(f"\nVersion: {gs1encoder.version}\n")

    # Set symbology to DataMatrix
    gs1encoder.sym = gs1encoders.Symbology.DM
    print(f"Symbology: {gs1encoder.sym.name}")

    # Demonstrate validation control
    print(
        "RequisiteAIs validation enabled: "
        f"{gs1encoder.get_validation_enabled(gs1encoders.Validation.REQUISITE_AIS)}"
    )
    gs1encoder.set_validation_enabled(gs1encoders.Validation.REQUISITE_AIS, False)
    print(
        "RequisiteAIs validation enabled (after disable): "
        f"{gs1encoder.get_validation_enabled(gs1encoders.Validation.REQUISITE_AIS)}"
    )

    # Set AI data
    ai_data = "(01)09521234543213(10)ABC123(99)TEST"
    gs1encoder.ai_data_str = ai_data
    print(f"\nAI IN:  {ai_data}")
    print(f"AI OUT: {gs1encoder.ai_data_str or 'Not AI data'}")
    print(f"DATA:   {gs1encoder.data_str}")

    # HRI with data titles
    gs1encoder.include_data_titles_in_hri = True
    title_suffix = " (including data titles)" if gs1encoder.include_data_titles_in_hri else ""
    print(f"\nHRI{title_suffix}:")
    for h in gs1encoder.hri:
        print(f"    {h}")

    # DL URI
    try:
        dl_uri = gs1encoder.get_dl_uri("https://example.com")
        print(f"\nDL URI: {dl_uri}")
    except GS1EncoderDigitalLinkException as e:
        print(f"\nDL URI: Failed: {e}")

    # Scan data round-trip
    if gs1encoder.scan_data:
        print(f"Scan data: {gs1encoder.scan_data}")
    else:
        print("Scan data: None")

    # DL ignored query params
    ignored = gs1encoder.dl_ignored_query_params
    if ignored:
        print("\nIgnored DL query params:")
        for p in ignored:
            print(f"    {p}")

    # Demonstrate boolean properties
    print(f"\nAdd check digit: {gs1encoder.add_check_digit}")
    print(f"Permit unknown AIs: {gs1encoder.permit_unknown_ais}")
    print(
        "Permit zero-suppressed GTIN in DL URIs: "
        f"{gs1encoder.permit_zero_suppressed_gtin_in_dl_uris}"
    )


if __name__ == "__main__":
    main()
