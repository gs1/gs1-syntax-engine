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

}
