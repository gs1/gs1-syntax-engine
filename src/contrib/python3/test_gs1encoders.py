#
#  Copyright (c) 2026 GS1 AISBL.
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

"""Tests for the Python 3 binding for the GS1 Barcode Syntax Engine."""

import unittest

from gs1encoders import (
    GS1Encoder,
    GS1EncoderDigitalLinkException,
    GS1EncoderParameterException,
    Symbology,
    Validation,
)


class GS1EncoderTest(unittest.TestCase):

    def test_set_dl_uri(self):
        gs1encoder = GS1Encoder()

        gs1encoder.data_str = "https://id.example.org/test/01/12312312312319?99=TESTING123"

        self.assertEqual(gs1encoder.data_str, "https://id.example.org/test/01/12312312312319?99=TESTING123")
        self.assertEqual(gs1encoder.get_dl_uri(), "https://id.gs1.org/01/12312312312319?99=TESTING123")
        self.assertEqual(gs1encoder.ai_data_str, "(01)12312312312319(99)TESTING123")

        gs1encoder.include_data_titles_in_hri = True
        self.assertEqual(gs1encoder.hri, ["GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"])

        gs1encoder.sym = Symbology.DM
        self.assertEqual(gs1encoder.sym, Symbology.DM)
        self.assertEqual(gs1encoder.scan_data, "]d1https://id.example.org/test/01/12312312312319?99=TESTING123")

    def test_set_ai_data_str(self):
        gs1encoder = GS1Encoder()

        gs1encoder.ai_data_str = "(01)12312312312319(99)TESTING123"

        self.assertEqual(gs1encoder.data_str, "^011231231231231999TESTING123")
        self.assertEqual(gs1encoder.get_dl_uri(), "https://id.gs1.org/01/12312312312319?99=TESTING123")
        self.assertEqual(gs1encoder.ai_data_str, "(01)12312312312319(99)TESTING123")
        self.assertEqual(gs1encoder.hri, ["(01) 12312312312319", "(99) TESTING123"])

        gs1encoder.sym = Symbology.QR
        self.assertEqual(gs1encoder.sym, Symbology.QR)
        self.assertEqual(gs1encoder.scan_data, "]Q3011231231231231999TESTING123")

    def test_requisites(self):
        gs1encoder = GS1Encoder()

        self.assertTrue(gs1encoder.get_validation_enabled(Validation.REQUISITE_AIS))

        with self.assertRaises(GS1EncoderParameterException) as ctx:
            gs1encoder.data_str = "^0212312312312319"
        self.assertIn("not satisfied", str(ctx.exception))

        gs1encoder.set_validation_enabled(Validation.REQUISITE_AIS, False)
        self.assertFalse(gs1encoder.get_validation_enabled(Validation.REQUISITE_AIS))
        gs1encoder.data_str = "^0212312312312319"

        self.assertEqual(gs1encoder.data_str, "^0212312312312319")
        with self.assertRaises(GS1EncoderDigitalLinkException) as ctx:
            gs1encoder.get_dl_uri()
        self.assertIn("without a primary key", str(ctx.exception))
        self.assertEqual(gs1encoder.ai_data_str, "(02)12312312312319")
        self.assertEqual(gs1encoder.hri, ["(02) 12312312312319"])

        gs1encoder.sym = Symbology.DATA_BAR_EXPANDED
        self.assertEqual(gs1encoder.sym, Symbology.DATA_BAR_EXPANDED)
        self.assertEqual(gs1encoder.scan_data, "]e00212312312312319")


if __name__ == "__main__":
    unittest.main()
