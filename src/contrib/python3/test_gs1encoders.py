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

        self.assertEqual(
            gs1encoder.data_str,
            "https://id.example.org/test/01/12312312312319?99=TESTING123",
        )
        self.assertEqual(
            gs1encoder.get_dl_uri(),
            "https://id.gs1.org/01/12312312312319?99=TESTING123",
        )
        self.assertEqual(gs1encoder.ai_data_str, "(01)12312312312319(99)TESTING123")

        gs1encoder.include_data_titles_in_hri = True
        self.assertEqual(gs1encoder.hri, ["GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"])

        gs1encoder.sym = Symbology.DM
        self.assertEqual(gs1encoder.sym, Symbology.DM)
        self.assertEqual(
            gs1encoder.scan_data,
            "]d1https://id.example.org/test/01/12312312312319?99=TESTING123",
        )

    def test_set_ai_data_str(self):
        gs1encoder = GS1Encoder()

        gs1encoder.ai_data_str = "(01)12312312312319(99)TESTING123"

        self.assertEqual(gs1encoder.data_str, "^011231231231231999TESTING123")
        self.assertEqual(
            gs1encoder.get_dl_uri(),
            "https://id.gs1.org/01/12312312312319?99=TESTING123",
        )
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

    def test_version(self):
        gs1encoder = GS1Encoder()

        self.assertTrue(gs1encoder.version)
        self.assertGreater(len(gs1encoder.version), 0)

    def test_defaults(self):
        gs1encoder = GS1Encoder()

        self.assertEqual(gs1encoder.sym, Symbology.NONE)
        self.assertEqual(gs1encoder.data_str, "")
        self.assertFalse(gs1encoder.add_check_digit)
        self.assertFalse(gs1encoder.permit_unknown_ais)
        self.assertFalse(gs1encoder.permit_zero_suppressed_gtin_in_dl_uris)
        self.assertFalse(gs1encoder.include_data_titles_in_hri)
        self.assertIsNone(gs1encoder.ai_data_str)
        self.assertIsNone(gs1encoder.scan_data)
        self.assertEqual(gs1encoder.hri, [])
        self.assertEqual(gs1encoder.dl_ignored_query_params, [])
        self.assertEqual(gs1encoder.err_markup, "")

    def test_boolean_setters(self):
        gs1encoder = GS1Encoder()

        self.assertFalse(gs1encoder.add_check_digit)
        gs1encoder.add_check_digit = True
        self.assertTrue(gs1encoder.add_check_digit)
        gs1encoder.add_check_digit = False
        self.assertFalse(gs1encoder.add_check_digit)

        self.assertFalse(gs1encoder.permit_unknown_ais)
        gs1encoder.permit_unknown_ais = True
        self.assertTrue(gs1encoder.permit_unknown_ais)
        gs1encoder.permit_unknown_ais = False
        self.assertFalse(gs1encoder.permit_unknown_ais)

        self.assertFalse(gs1encoder.permit_zero_suppressed_gtin_in_dl_uris)
        gs1encoder.permit_zero_suppressed_gtin_in_dl_uris = True
        self.assertTrue(gs1encoder.permit_zero_suppressed_gtin_in_dl_uris)
        gs1encoder.permit_zero_suppressed_gtin_in_dl_uris = False
        self.assertFalse(gs1encoder.permit_zero_suppressed_gtin_in_dl_uris)

        self.assertFalse(gs1encoder.include_data_titles_in_hri)
        gs1encoder.include_data_titles_in_hri = True
        self.assertTrue(gs1encoder.include_data_titles_in_hri)
        gs1encoder.include_data_titles_in_hri = False
        self.assertFalse(gs1encoder.include_data_titles_in_hri)

    def test_validations(self):
        gs1encoder = GS1Encoder()

        self.assertTrue(gs1encoder.get_validation_enabled(Validation.MUTEX_AIS))
        self.assertTrue(gs1encoder.get_validation_enabled(Validation.REQUISITE_AIS))
        self.assertTrue(gs1encoder.get_validation_enabled(Validation.REPEATED_AIS))
        self.assertTrue(gs1encoder.get_validation_enabled(Validation.DIG_SIG_SERIAL_KEY))
        self.assertTrue(gs1encoder.get_validation_enabled(Validation.UNKNOWN_AI_NOT_DL_ATTR))

        gs1encoder.set_validation_enabled(Validation.REQUISITE_AIS, False)
        self.assertFalse(gs1encoder.get_validation_enabled(Validation.REQUISITE_AIS))
        gs1encoder.set_validation_enabled(Validation.REQUISITE_AIS, True)
        self.assertTrue(gs1encoder.get_validation_enabled(Validation.REQUISITE_AIS))

        with self.assertRaises(GS1EncoderParameterException):
            gs1encoder.set_validation_enabled(Validation.REPEATED_AIS, False)

    def test_set_scan_data(self):
        gs1encoder = GS1Encoder()

        gs1encoder.scan_data = "]e0011231231231233310ABC123\x1d99XYZ"
        self.assertEqual(gs1encoder.sym, Symbology.DATA_BAR_EXPANDED)
        self.assertEqual(gs1encoder.data_str, "^011231231231233310ABC123^99XYZ")
        self.assertEqual(gs1encoder.ai_data_str, "(01)12312312312333(10)ABC123(99)XYZ")

    def test_non_ai_data(self):
        gs1encoder = GS1Encoder()

        gs1encoder.data_str = "TESTING"
        self.assertIsNone(gs1encoder.ai_data_str)
        self.assertIsNone(gs1encoder.scan_data)
        self.assertEqual(gs1encoder.hri, [])
        self.assertEqual(gs1encoder.dl_ignored_query_params, [])
        with self.assertRaises(GS1EncoderDigitalLinkException) as ctx:
            gs1encoder.get_dl_uri()
        self.assertIn("without a primary key", str(ctx.exception))

    def test_get_dl_uri_with_stem(self):
        gs1encoder = GS1Encoder()

        gs1encoder.ai_data_str = "(01)12312312312319"
        self.assertTrue(
            gs1encoder.get_dl_uri("https://example.com").startswith("https://example.com/")
        )
        self.assertTrue(gs1encoder.get_dl_uri().startswith("https://id.gs1.org/"))

    def test_dl_ignored_query_params(self):
        gs1encoder = GS1Encoder()

        gs1encoder.data_str = "https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ"
        self.assertEqual(gs1encoder.dl_ignored_query_params, ["singleton", "compound=XYZ"])
        self.assertEqual(gs1encoder.hri, ["(01) 12312312312333", "(22) TESTING", "(99) ABC"])

    def test_err_markup(self):
        gs1encoder = GS1Encoder()

        with self.assertRaises(GS1EncoderParameterException) as ctx:
            gs1encoder.data_str = "^011234567890128399ABC"
        self.assertIn("check digit", str(ctx.exception))
        self.assertNotEqual(gs1encoder.err_markup, "")
        self.assertIn("|", gs1encoder.err_markup)

    def test_del(self):
        gs1encoder = GS1Encoder()
        self.assertTrue(gs1encoder.version)
        del gs1encoder  # Should trigger __del__ and free native resources

    def test_context_manager(self):
        with GS1Encoder() as gs1encoder:
            self.assertTrue(gs1encoder.version)


if __name__ == "__main__":
    unittest.main()
