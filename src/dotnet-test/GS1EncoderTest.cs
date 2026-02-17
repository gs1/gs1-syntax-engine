using Microsoft.VisualStudio.TestTools.UnitTesting;
using GS1.Encoders;

namespace GS1EncodersTest
{

    /*
     * Copyright (c) 2026 GS1 AISBL.
     *
     * Licensed under the Apache License, Version 2.0 (the "License");
     * you may not use this file except in compliance with the License.
     *
     * You may obtain a copy of the License at
     *
     *     http://www.apache.org/licenses/LICENSE-2.0
     *
     * Unless required by applicable law or agreed to in writing, software
     * distributed under the License is distributed on an "AS IS" BASIS,
     * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
     * See the License for the specific language governing permissions and
     * limitations under the License.
     *
     */

    [TestClass]
    public class GS1EncoderTest
    {

        [TestMethod]
        public void TestSetDLuri()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.DataStr = "https://id.example.org/test/01/12312312312319?99=TESTING123";

            Assert.AreEqual("https://id.example.org/test/01/12312312312319?99=TESTING123", gs1encoder.DataStr);
            Assert.AreEqual("https://id.gs1.org/01/12312312312319?99=TESTING123", gs1encoder.GetDLuri(null));
            Assert.AreEqual("(01)12312312312319(99)TESTING123", gs1encoder.AIdataStr);

            gs1encoder.IncludeDataTitlesInHRI = true;
            CollectionAssert.AreEqual(new string[] { "GTIN (01) 12312312312319", "INTERNAL (99) TESTING123" }, gs1encoder.HRI);

            gs1encoder.Sym = GS1Encoder.Symbology.DM;
            Assert.AreEqual(GS1Encoder.Symbology.DM, gs1encoder.Sym);
            Assert.AreEqual("]d1https://id.example.org/test/01/12312312312319?99=TESTING123", gs1encoder.ScanData);
        }

        [TestMethod]
        public void TestSetAIdataStr()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.AIdataStr = "(01)12312312312319(99)TESTING123";

            Assert.AreEqual("^011231231231231999TESTING123", gs1encoder.DataStr);
            Assert.AreEqual("https://id.gs1.org/01/12312312312319?99=TESTING123", gs1encoder.GetDLuri(null));
            Assert.AreEqual("(01)12312312312319(99)TESTING123", gs1encoder.AIdataStr);
            CollectionAssert.AreEqual(new string[] { "(01) 12312312312319", "(99) TESTING123" }, gs1encoder.HRI);

            gs1encoder.Sym = GS1Encoder.Symbology.QR;
            Assert.AreEqual(GS1Encoder.Symbology.QR, gs1encoder.Sym);
            Assert.AreEqual("]Q3011231231231231999TESTING123", gs1encoder.ScanData);
        }

        [TestMethod]
        public void TestRequisites()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RequisiteAIs));

            GS1EncoderParameterException e1 = Assert.ThrowsExactly<GS1EncoderParameterException>(() =>
            {
                gs1encoder.DataStr = "^0212312312312319";
            });
            StringAssert.Contains(e1.Message, "not satisfied");

            gs1encoder.SetValidationEnabled(GS1Encoder.Validation.RequisiteAIs, false);
            Assert.IsFalse(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
            gs1encoder.DataStr = "^0212312312312319";

            Assert.AreEqual("^0212312312312319", gs1encoder.DataStr);

            GS1EncoderDigitalLinkException e2 = Assert.ThrowsExactly<GS1EncoderDigitalLinkException>(() =>
            {
                gs1encoder.GetDLuri(null);
            });
            StringAssert.Contains(e2.Message, "without a primary key");

            Assert.AreEqual("(02)12312312312319", gs1encoder.AIdataStr);
            CollectionAssert.AreEqual(new string[] { "(02) 12312312312319" }, gs1encoder.HRI);

            gs1encoder.Sym = GS1Encoder.Symbology.DataBarExpanded;
            Assert.AreEqual(GS1Encoder.Symbology.DataBarExpanded, gs1encoder.Sym);
            Assert.AreEqual("]e00212312312312319", gs1encoder.ScanData);
        }

        [TestMethod]
        public void TestVersion()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            Assert.IsNotNull(gs1encoder.Version);
            Assert.AreNotEqual("", gs1encoder.Version);
        }

        [TestMethod]
        public void TestDefaults()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            Assert.AreEqual(GS1Encoder.Symbology.NONE, gs1encoder.Sym);
            Assert.AreEqual("", gs1encoder.DataStr);
            Assert.IsFalse(gs1encoder.AddCheckDigit);
            Assert.IsFalse(gs1encoder.PermitUnknownAIs);
            Assert.IsFalse(gs1encoder.PermitZeroSuppressedGTINinDLuris);
            Assert.IsFalse(gs1encoder.IncludeDataTitlesInHRI);
            Assert.IsNull(gs1encoder.AIdataStr);
            Assert.IsNull(gs1encoder.ScanData);
            Assert.AreEqual(0, gs1encoder.HRI.Length);
            Assert.AreEqual(0, gs1encoder.DLignoredQueryParams.Length);
            Assert.AreEqual("", gs1encoder.ErrMarkup);
        }

        [TestMethod]
        public void TestBooleanSetters()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            Assert.IsFalse(gs1encoder.AddCheckDigit);
            gs1encoder.AddCheckDigit = true;
            Assert.IsTrue(gs1encoder.AddCheckDigit);
            gs1encoder.AddCheckDigit = false;
            Assert.IsFalse(gs1encoder.AddCheckDigit);

            Assert.IsFalse(gs1encoder.PermitUnknownAIs);
            gs1encoder.PermitUnknownAIs = true;
            Assert.IsTrue(gs1encoder.PermitUnknownAIs);
            gs1encoder.PermitUnknownAIs = false;
            Assert.IsFalse(gs1encoder.PermitUnknownAIs);

            Assert.IsFalse(gs1encoder.PermitZeroSuppressedGTINinDLuris);
            gs1encoder.PermitZeroSuppressedGTINinDLuris = true;
            Assert.IsTrue(gs1encoder.PermitZeroSuppressedGTINinDLuris);
            gs1encoder.PermitZeroSuppressedGTINinDLuris = false;
            Assert.IsFalse(gs1encoder.PermitZeroSuppressedGTINinDLuris);

            Assert.IsFalse(gs1encoder.IncludeDataTitlesInHRI);
            gs1encoder.IncludeDataTitlesInHRI = true;
            Assert.IsTrue(gs1encoder.IncludeDataTitlesInHRI);
            gs1encoder.IncludeDataTitlesInHRI = false;
            Assert.IsFalse(gs1encoder.IncludeDataTitlesInHRI);
        }

        [TestMethod]
        public void TestValidations()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.MutexAIs));
            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RepeatedAIs));
            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.DigSigSerialKey));
            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.UnknownAInotDLattr));

            gs1encoder.SetValidationEnabled(GS1Encoder.Validation.RequisiteAIs, false);
            Assert.IsFalse(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
            gs1encoder.SetValidationEnabled(GS1Encoder.Validation.RequisiteAIs, true);
            Assert.IsTrue(gs1encoder.GetValidationEnabled(GS1Encoder.Validation.RequisiteAIs));

            Assert.ThrowsExactly<GS1EncoderParameterException>(() =>
            {
                gs1encoder.SetValidationEnabled(GS1Encoder.Validation.RepeatedAIs, false);
            });
        }

        [TestMethod]
        public void TestSetScanData()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.ScanData = "]e0011231231231233310ABC123\u001D99XYZ";
            Assert.AreEqual(GS1Encoder.Symbology.DataBarExpanded, gs1encoder.Sym);
            Assert.AreEqual("^011231231231233310ABC123^99XYZ", gs1encoder.DataStr);
            Assert.AreEqual("(01)12312312312333(10)ABC123(99)XYZ", gs1encoder.AIdataStr);
        }

        [TestMethod]
        public void TestNonAIdata()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.DataStr = "TESTING";
            Assert.IsNull(gs1encoder.AIdataStr);
            Assert.IsNull(gs1encoder.ScanData);
            Assert.AreEqual(0, gs1encoder.HRI.Length);
            Assert.AreEqual(0, gs1encoder.DLignoredQueryParams.Length);

            GS1EncoderDigitalLinkException e = Assert.ThrowsExactly<GS1EncoderDigitalLinkException>(() =>
            {
                gs1encoder.GetDLuri(null);
            });
            StringAssert.Contains(e.Message, "without a primary key");
        }

        [TestMethod]
        public void TestGetDLuriWithStem()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.AIdataStr = "(01)12312312312319";
            Assert.IsTrue(gs1encoder.GetDLuri("https://example.com").StartsWith("https://example.com/"));
            Assert.IsTrue(gs1encoder.GetDLuri(null).StartsWith("https://id.gs1.org/"));
        }

        [TestMethod]
        public void TestDLignoredQueryParams()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            gs1encoder.DataStr = "https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ";
            CollectionAssert.AreEqual(new string[] { "singleton", "compound=XYZ" }, gs1encoder.DLignoredQueryParams);
            CollectionAssert.AreEqual(new string[] { "(01) 12312312312333", "(22) TESTING", "(99) ABC" }, gs1encoder.HRI);
        }

        [TestMethod]
        public void TestDispose()
        {
            using (GS1Encoder gs1encoder = new GS1Encoder())
            {
                Assert.IsNotNull(gs1encoder.Version);
            }
        }

        [TestMethod]
        public void TestErrMarkup()
        {
            GS1Encoder gs1encoder = new GS1Encoder();

            GS1EncoderParameterException e = Assert.ThrowsExactly<GS1EncoderParameterException>(() =>
            {
                gs1encoder.DataStr = "^011234567890128399ABC";
            });
            StringAssert.Contains(e.Message, "check digit");
            Assert.AreNotEqual("", gs1encoder.ErrMarkup);
            StringAssert.Contains(gs1encoder.ErrMarkup, "|");
        }

    }

}
