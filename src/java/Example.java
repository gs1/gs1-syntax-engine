/**
 * GS1 Barcode Syntax Engine example user of the Java binding
 *
 * @author Copyright (c) 2022-2026 GS1 AISBL.
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

import org.gs1.gs1encoders.*;

import java.util.Scanner;


public class Example {

    public static void main(final String args[]) {

        GS1Encoder gs1encoder = null;

        // Load the GS1 Syntax Dictionary from local storage (here, the current
        // directory), falling back to the AI table embedded in the library if
        // the file is absent or malformed. Replacing the file with a newer
        // revision of the Syntax Dictionary lets the application adopt it
        // without rebuilding the library.
        try {
            GS1Encoder.InitOptions options = new GS1Encoder.InitOptions()
                    .setSyntaxDictionary("gs1-syntax-dictionary.txt")
                    .setFallbackOnSyndictError(true);
            gs1encoder = new GS1Encoder(options);
        } catch (GS1EncoderGeneralException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        }

        if (gs1encoder.getInitFallbackWarning() != null)
            System.err.format("Warning: %s\n", gs1encoder.getInitFallbackWarning());

        boolean exit = false;

        if (args.length == 1 && args[0].equals("--version")) {
            System.out.format("Library version: %s\n", gs1encoder.getVersion());
            exit = true;
        }

        Scanner scanner = new Scanner(System.in);

        while (!exit) {

            String dataStr = gs1encoder.getDataStr();
            String aiDataStr = "";
            String dlURI = "";
            String[] hri = {};
            if (!dataStr.isEmpty()) {
                aiDataStr = gs1encoder.getAIdataStr();
                if (aiDataStr == null) aiDataStr = "⧚ Not AI-based data ⧚";
                try { dlURI = gs1encoder.getDLuri(null); } catch (GS1EncoderDigitalLinkException e) { dlURI = "⧚ " + e.getMessage() + " ⧚"; }
                hri = gs1encoder.getHRI();
            }

            System.out.println("\n\n\nCurrent state:");

            System.out.format("\n    Barcode message:        %s", dataStr);
            System.out.format("\n    AI element string:      %s", aiDataStr);
            System.out.format("\n    GS1 Digital Link URI:   %s", dlURI);
            System.out.format("\n    HRI:                    %s\n", !dataStr.isEmpty() && hri.length == 0 ? "⧚ Not AI-based data ⧚": "");
            for (String h : hri) {
                System.out.format("       %s\n", h);
            }

            System.out.println("\n\nMENU:");
            System.out.println("\n 1) Process raw barcode message data, either:");
            System.out.println("      * Plain data");
            System.out.println("      * Unbracketed AI element string with FNC1 in first position");
            System.out.println("      * GS1 Digital Link URI");
            System.out.println(" 2) Process a bracketed AI element string");
            System.out.println(" 3) Process barcode scan data (prefixed by AIM Symbology Identifier)");

            System.out.format("\n 4) Toggle 'include data titles in HRI' flag.  Current value = %s\n",
                                                gs1encoder.getIncludeDataTitlesInHRI() ? "ON" : "OFF");
            System.out.format(" 5) Toggle 'permit unknown AIs' flag.          Current value = %s\n",
                                                gs1encoder.getPermitUnknownAIs() ? "ON" : "OFF");
            System.out.format(" 6) Toggle 'validate AI requisites'.           Current value = %s\n",
                                                gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs) ? "ON" : "OFF");

            System.out.println("\n 0) Exit program");

            System.out.print("\nMenu selection: ");
            String menuVal = scanner.nextLine();

            String inpStr;

            switch (menuVal) {
                case "1":
                case "2":
                case "3":
                    System.out.print("\nEnter data: ");
                    inpStr = scanner.nextLine();
                    if (inpStr.isEmpty())
                        continue;

                    try {
                        if (menuVal.equals("1"))
                            gs1encoder.setDataStr(inpStr);
                        else if (menuVal.equals("2"))
                            gs1encoder.setAIdataStr(inpStr);
                        else            // "3"
                            gs1encoder.setScanData(inpStr);
                    } catch (GS1EncoderParameterException | GS1EncoderScanDataException e) {
                        System.out.format("\n\nERROR message: %s\n\n", e.getMessage());
                        String markup = gs1encoder.getErrMarkup();
                        if (!markup.isEmpty())
                            System.out.format("ERROR markup:  %s\n", markup.replace("|", "⧚"));
                        continue;
                    }
                    break;
                case "4":
                case "5":
                case "6":
                    System.out.print("\nEnter 0 for OFF or 1 for ON: ");
                    inpStr = scanner.nextLine();
                    if (inpStr.isEmpty())
                        continue;

                    if (!inpStr.equals("0") && !inpStr.equals("1")) {
                        System.out.println("\n\nOUT OF RANGE. PLEASE ENTER 0 or 1");
                        continue;
                    }
                    try {
                        if (menuVal.equals("4"))
                            gs1encoder.setIncludeDataTitlesInHRI(inpStr.equals("1"));
                        else if (menuVal.equals("5"))
                            gs1encoder.setPermitUnknownAIs(inpStr.equals("1"));
                        else           // "6"
                            gs1encoder.setValidationEnabled(GS1Encoder.Validation.RequisiteAIs, inpStr.equals("1"));
                    } catch (GS1EncoderParameterException e) {
                        System.out.format("\n\nERROR: %s\n", e.getMessage());
                        continue;
                    }
                    break;
                case "0":
                    exit = true;
                    break;
                default:
                    System.out.println("\n\nOUT OF RANGE. PLEASE ENTER 1-6, 9 or 0.");
            }


        }

        gs1encoder.free();

    }

}
