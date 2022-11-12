/**
 * GS1 Syntax Engine example user of the Java binding
 *
 * @author Copyright (c) 2022 GS1 AISBL.
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

import org.gs1.gs1encoders.GS1Encoder;

public class Example {

    static {
        System.loadLibrary("gs1encoders");
    }

    public static void main(final String args[]) {

        try {

            String intext = "^011231231231232699ABC";

            GS1Encoder gs1encoder = new GS1Encoder();

            System.out.println("Library version: " + gs1encoder.getVersion() + "\n");

            System.out.println("IN: " + intext + "\n");

            gs1encoder.setDataStr(intext);
            System.out.println("AI: " + gs1encoder.getDataStr() + "\n");
            System.out.println("DL: " + gs1encoder.getDLuri("https://example.org/") + "\n");

            gs1encoder.setIncludeDataTitlesInHRI(true);

            System.out.println("HRI (with data titles):\n");
            for (String hri: gs1encoder.getHRI())
                System.out.println(hri);

            gs1encoder.free();

        } catch (Exception e) {

            System.err.println(e.getMessage());
            System.exit(1);

        }

    }

}
