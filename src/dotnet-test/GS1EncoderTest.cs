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

    }

}
