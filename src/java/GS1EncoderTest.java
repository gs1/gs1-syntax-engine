/**
 *  Basic tests for the Java binding for the GS1 Barcode Syntax Engine.
 *
 *  Copyright (c) 2026 GS1 AISBL.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

import org.gs1.gs1encoders.*;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class GS1EncoderTest {

    static {
        System.loadLibrary("gs1encodersjni");
    }

    @Test
    public void testSetDLuri() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setDataStr("https://id.example.org/test/01/12312312312319?99=TESTING123");

        assertEquals("https://id.example.org/test/01/12312312312319?99=TESTING123", gs1encoder.getDataStr());
        assertEquals("https://id.gs1.org/01/12312312312319?99=TESTING123", gs1encoder.getDLuri(null));
        assertEquals("(01)12312312312319(99)TESTING123", gs1encoder.getAIdataStr());

        gs1encoder.setIncludeDataTitlesInHRI(true);
        assertArrayEquals(new String[]{"GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"}, gs1encoder.getHRI());

        gs1encoder.setSym(GS1Encoder.Symbology.DM);
        assertEquals(GS1Encoder.Symbology.DM, gs1encoder.getSym());
        assertEquals("]d1https://id.example.org/test/01/12312312312319?99=TESTING123", gs1encoder.getScanData());

        gs1encoder.free();
    }

    @Test
    public void testSetAIdataStr() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setAIdataStr("(01)12312312312319(99)TESTING123");

        assertEquals("^011231231231231999TESTING123", gs1encoder.getDataStr());
        assertEquals("https://id.gs1.org/01/12312312312319?99=TESTING123", gs1encoder.getDLuri(null));
        assertEquals("(01)12312312312319(99)TESTING123", gs1encoder.getAIdataStr());
        assertArrayEquals(new String[]{"(01) 12312312312319", "(99) TESTING123"}, gs1encoder.getHRI());

        gs1encoder.setSym(GS1Encoder.Symbology.QR);
        assertEquals(GS1Encoder.Symbology.QR, gs1encoder.getSym());
        assertEquals("]Q3011231231231231999TESTING123", gs1encoder.getScanData());

        gs1encoder.free();
    }

    @Test
    public void testRequisites() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
        GS1EncoderParameterException e1 = assertThrows(GS1EncoderParameterException.class, () -> {
            gs1encoder.setDataStr("^0212312312312319");
        });
        assertTrue(e1.getMessage().contains("not satisfied"));

        gs1encoder.setValidationEnabled(GS1Encoder.Validation.RequisiteAIs, false);
        assertFalse(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
        gs1encoder.setDataStr("^0212312312312319");

        assertEquals("^0212312312312319", gs1encoder.getDataStr());
        GS1EncoderDigitalLinkException e2 = assertThrows(GS1EncoderDigitalLinkException.class, () -> {
            gs1encoder.getDLuri(null);
        });
        assertTrue(e2.getMessage().contains("without a primary key"));
        assertEquals("(02)12312312312319", gs1encoder.getAIdataStr());
        assertArrayEquals(new String[]{"(02) 12312312312319"}, gs1encoder.getHRI());

        gs1encoder.setSym(GS1Encoder.Symbology.DataBarExpanded);
        assertEquals(GS1Encoder.Symbology.DataBarExpanded, gs1encoder.getSym());
        assertEquals("]e00212312312312319", gs1encoder.getScanData());

        gs1encoder.free();
    }

    @Test
    public void testVersion() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        assertNotNull(gs1encoder.getVersion());
        assertFalse(gs1encoder.getVersion().isEmpty());

        gs1encoder.free();
    }

    @Test
    public void testDefaults() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        assertEquals(GS1Encoder.Symbology.NONE, gs1encoder.getSym());
        assertEquals("", gs1encoder.getDataStr());
        assertFalse(gs1encoder.getAddCheckDigit());
        assertFalse(gs1encoder.getPermitUnknownAIs());
        assertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris());
        assertFalse(gs1encoder.getIncludeDataTitlesInHRI());
        assertNull(gs1encoder.getAIdataStr());
        assertNull(gs1encoder.getScanData());
        assertArrayEquals(new String[]{}, gs1encoder.getHRI());
        assertArrayEquals(new String[]{}, gs1encoder.getDLignoredQueryParams());
        assertEquals("", gs1encoder.getErrMarkup());

        gs1encoder.free();
    }

    @Test
    public void testBooleanSetters() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        assertFalse(gs1encoder.getAddCheckDigit());
        gs1encoder.setAddCheckDigit(true);
        assertTrue(gs1encoder.getAddCheckDigit());
        gs1encoder.setAddCheckDigit(false);
        assertFalse(gs1encoder.getAddCheckDigit());

        assertFalse(gs1encoder.getPermitUnknownAIs());
        gs1encoder.setPermitUnknownAIs(true);
        assertTrue(gs1encoder.getPermitUnknownAIs());
        gs1encoder.setPermitUnknownAIs(false);
        assertFalse(gs1encoder.getPermitUnknownAIs());

        assertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris());
        gs1encoder.setPermitZeroSuppressedGTINinDLuris(true);
        assertTrue(gs1encoder.getPermitZeroSuppressedGTINinDLuris());
        gs1encoder.setPermitZeroSuppressedGTINinDLuris(false);
        assertFalse(gs1encoder.getPermitZeroSuppressedGTINinDLuris());

        assertFalse(gs1encoder.getIncludeDataTitlesInHRI());
        gs1encoder.setIncludeDataTitlesInHRI(true);
        assertTrue(gs1encoder.getIncludeDataTitlesInHRI());
        gs1encoder.setIncludeDataTitlesInHRI(false);
        assertFalse(gs1encoder.getIncludeDataTitlesInHRI());

        gs1encoder.free();
    }

    @Test
    public void testValidations() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.MutexAIs));
        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RepeatedAIs));
        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.DigSigSerialKey));
        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.UnknownAInotDLattr));

        gs1encoder.setValidationEnabled(GS1Encoder.Validation.RequisiteAIs, false);
        assertFalse(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs));
        gs1encoder.setValidationEnabled(GS1Encoder.Validation.RequisiteAIs, true);
        assertTrue(gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs));

        assertThrows(GS1EncoderParameterException.class, () -> {
            gs1encoder.setValidationEnabled(GS1Encoder.Validation.RepeatedAIs, false);
        });

        gs1encoder.free();
    }

    @Test
    public void testSetScanData() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setScanData("]e0011231231231233310ABC123\u001D99XYZ");
        assertEquals(GS1Encoder.Symbology.DataBarExpanded, gs1encoder.getSym());
        assertEquals("^011231231231233310ABC123^99XYZ", gs1encoder.getDataStr());
        assertEquals("(01)12312312312333(10)ABC123(99)XYZ", gs1encoder.getAIdataStr());

        gs1encoder.free();
    }

    @Test
    public void testNonAIdata() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setDataStr("TESTING");
        assertNull(gs1encoder.getAIdataStr());
        assertNull(gs1encoder.getScanData());
        assertArrayEquals(new String[]{}, gs1encoder.getHRI());
        assertArrayEquals(new String[]{}, gs1encoder.getDLignoredQueryParams());
        GS1EncoderDigitalLinkException e = assertThrows(GS1EncoderDigitalLinkException.class, () -> {
            gs1encoder.getDLuri(null);
        });
        assertTrue(e.getMessage().contains("without a primary key"));

        gs1encoder.free();
    }

    @Test
    public void testGetDLuriWithStem() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setAIdataStr("(01)12312312312319");
        assertTrue(gs1encoder.getDLuri("https://example.com").startsWith("https://example.com/"));
        assertTrue(gs1encoder.getDLuri(null).startsWith("https://id.gs1.org/"));

        gs1encoder.free();
    }

    @Test
    public void testDLignoredQueryParams() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        gs1encoder.setDataStr("https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ");
        assertArrayEquals(new String[]{"singleton", "compound=XYZ"}, gs1encoder.getDLignoredQueryParams());
        assertArrayEquals(new String[]{"(01) 12312312312333", "(22) TESTING", "(99) ABC"}, gs1encoder.getHRI());

        gs1encoder.free();
    }

    @Test
    public void testAutoCloseable() throws Exception {
        try (GS1Encoder gs1encoder = new GS1Encoder()) {
            gs1encoder.setAIdataStr("(01)12312312312319");
            assertTrue(gs1encoder.getDLuri(null).startsWith("https://id.gs1.org/"));
        }
    }

    @Test
    public void testErrMarkup() throws Exception {
        GS1Encoder gs1encoder = new GS1Encoder();

        GS1EncoderParameterException e = assertThrows(GS1EncoderParameterException.class, () -> {
            gs1encoder.setDataStr("^011234567890128399ABC");
        });
        assertTrue(e.getMessage().contains("check digit"));
        assertFalse(gs1encoder.getErrMarkup().isEmpty());
        assertTrue(gs1encoder.getErrMarkup().contains("|"));

        gs1encoder.free();
    }

}
