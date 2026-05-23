/**
 * GS1 Barcode Syntax Engine example user of the C API
 *
 * @author Copyright (c) 2026 GS1 AISBL.
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

#include <stdbool.h>
#include <stdio.h>

#include "gs1encoders.h"


int main(void) {

	gs1_encoder *gs1encoder;
	const char *aiData = "(01)09521234543213(10)ABC123(99)TEST";
	const char *aiDataOut, *dlURI, *scanData;
	char **hri, **ignored;
	int numHRI, numIgnored, i;

	gs1encoder = gs1_encoder_init_ex(NULL, NULL);
	if (!gs1encoder) {
		fprintf(stderr, "Failed to initialise the GS1 Barcode Syntax Engine\n");
		return 1;
	}

	printf("\nVersion: %s\n\n", gs1_encoder_getVersion());

	// Set symbology to DataMatrix (no symbology-name accessor in the C API)
	gs1_encoder_setSym(gs1encoder, gs1_encoder_sDM);
	printf("Symbology: %d\n", gs1_encoder_getSym(gs1encoder));

	// Demonstrate validation control
	printf("RequisiteAIs validation enabled: %s\n",
		gs1_encoder_getValidationEnabled(gs1encoder, gs1_encoder_vREQUISITE_AIS) ? "true" : "false");
	gs1_encoder_setValidationEnabled(gs1encoder, gs1_encoder_vREQUISITE_AIS, false);
	printf("RequisiteAIs validation enabled (after disable): %s\n",
		gs1_encoder_getValidationEnabled(gs1encoder, gs1_encoder_vREQUISITE_AIS) ? "true" : "false");

	// Set AI data
	if (!gs1_encoder_setAIdataStr(gs1encoder, aiData)) {
		fprintf(stderr, "%s\n", gs1_encoder_getErrMsg(gs1encoder));
		gs1_encoder_free(gs1encoder);
		return 1;
	}
	aiDataOut = gs1_encoder_getAIdataStr(gs1encoder);
	printf("\nAI IN:  %s\n", aiData);
	printf("AI OUT: %s\n", aiDataOut ? aiDataOut : "Not AI data");
	printf("DATA:   %s\n", gs1_encoder_getDataStr(gs1encoder));

	// HRI with data titles
	gs1_encoder_setIncludeDataTitlesInHRI(gs1encoder, true);
	printf("\nHRI%s:\n",
		gs1_encoder_getIncludeDataTitlesInHRI(gs1encoder) ? " (including data titles)" : "");
	numHRI = gs1_encoder_getHRI(gs1encoder, &hri);
	for (i = 0; i < numHRI; i++)
		printf("    %s\n", hri[i]);

	// DL URI
	dlURI = gs1_encoder_getDLuri(gs1encoder, "https://example.com");
	if (dlURI)
		printf("\nDL URI: %s\n", dlURI);
	else
		printf("\nDL URI: Failed: %s\n", gs1_encoder_getErrMsg(gs1encoder));

	// Scan data round-trip
	scanData = gs1_encoder_getScanData(gs1encoder);
	printf("Scan data: %s\n", (scanData && *scanData) ? scanData : "None");

	// DL ignored query params
	numIgnored = gs1_encoder_getDLignoredQueryParams(gs1encoder, &ignored);
	if (numIgnored > 0) {
		printf("\nIgnored DL query params:\n");
		for (i = 0; i < numIgnored; i++)
			printf("    %s\n", ignored[i]);
	}

	// Demonstrate boolean properties
	printf("\nAdd check digit: %s\n",
		gs1_encoder_getAddCheckDigit(gs1encoder) ? "true" : "false");
	printf("Permit unknown AIs: %s\n",
		gs1_encoder_getPermitUnknownAIs(gs1encoder) ? "true" : "false");
	printf("Permit zero-suppressed GTIN in DL URIs: %s\n",
		gs1_encoder_getPermitZeroSuppressedGTINinDLuris(gs1encoder) ? "true" : "false");

	gs1_encoder_free(gs1encoder);

	return 0;
}
