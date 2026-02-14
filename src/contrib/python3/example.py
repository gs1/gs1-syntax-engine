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

from gs1encoders import GS1Encoder, Symbology, Validation


def main():

    gs1encoder = GS1Encoder()

    print("\nVersion: {}\n".format(gs1encoder.get_version()))

    # Set symbology to DataMatrix
    gs1encoder.set_sym(Symbology.DM)
    print("Symbology: {}".format(gs1encoder.get_sym().name))

    # Demonstrate validation control
    print("RequisiteAIs validation enabled: {}".format(gs1encoder.get_validation_enabled(Validation.RequisiteAIs)))
    gs1encoder.set_validation_enabled(Validation.RequisiteAIs, False)
    print(
        "RequisiteAIs validation enabled (after disable): {}".format(
            gs1encoder.get_validation_enabled(Validation.RequisiteAIs)
        )
    )

    # Set AI data
    ai_data = "(01)09521234543213(10)ABC123(99)TEST"
    gs1encoder.set_ai_data_str(ai_data)
    print("\nAI IN:  {}".format(ai_data))
    print("AI OUT: {}".format(gs1encoder.get_ai_data_str() or "Not AI data"))
    print("DATA:   {}".format(gs1encoder.get_data_str()))

    # HRI with data titles
    gs1encoder.set_include_data_titles_in_hri(True)
    print("\nHRI{}:".format(" (including data titles)" if gs1encoder.get_include_data_titles_in_hri() else ""))
    for h in gs1encoder.get_hri():
        print("    {}".format(h))

    # DL URI
    dl_uri = gs1encoder.get_dl_uri("https://example.com")
    if dl_uri:
        print("\nDL URI: {}".format(dl_uri))
    else:
        print("\nDL URI: Failed: {}".format(gs1encoder.get_err_markup()))

    # Scan data round-trip
    scan_data = gs1encoder.get_scan_data()
    if scan_data:
        print("Scan data: {}".format(scan_data))
    else:
        print("Scan data: None")

    # DL ignored query params
    ignored = gs1encoder.get_dl_ignored_query_params()
    if ignored:
        print("\nIgnored DL query params:")
        for p in ignored:
            print("    {}".format(p))

    # Demonstrate boolean properties
    print("\nAdd check digit: {}".format(gs1encoder.get_add_check_digit()))
    print("Permit unknown AIs: {}".format(gs1encoder.get_permit_unknown_ais()))
    print("Permit zero-suppressed GTIN in DL URIs: {}".format(gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris()))


if __name__ == "__main__":
    main()
