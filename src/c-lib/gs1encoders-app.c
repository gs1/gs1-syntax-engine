/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2000-2024 GS1 AISBL.
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
#include <stdlib.h>
#include <string.h>

#include "gs1encoders.h"

#define RELEASE __DATE__

static char *inpStr;

// Replacement for the deprecated gets(3) function
#ifdef gets
#undef gets
#endif
#define gets(i) _gets(i)

static char* _gets(char* const in) {

	char* s;

	s = fgets(in, gs1_encoder_getMaxDataStrLength()+1, stdin);
	if (s != NULL) {
		s[strcspn(s, "\r\n")] = 0;
	}
	return s;
}

static bool userInt(gs1_encoder* const ctx) {

	while (true) {

		bool ret = false;
		const char *dataStr, *aiDataStr, *dlURI = NULL;
		char **hri = NULL;
		int menuVal, numHRI, i;

		printf("\n\n\nCurrent state:\n");

		dataStr = gs1_encoder_getDataStr(ctx);
		printf("\n    Barcode message:        %s", dataStr);

		aiDataStr = "";
		if (*dataStr != '\0') aiDataStr = gs1_encoder_getAIdataStr(ctx);
		printf("\n    AI element string:      %s", aiDataStr ? aiDataStr : "⧚ Not AI-based data ⧚");

		dlURI = "";
		if (*dataStr != '\0') dlURI = gs1_encoder_getDLuri(ctx, NULL);
		if (dlURI) {
			printf("\n    GS1 Digital Link URI:   %s", dlURI);
		} else {
			printf("\n    GS1 Digital Link URI:   ⧚ %s ⧚", gs1_encoder_getErrMsg(ctx));
		}

		numHRI = 0;
		if (*dataStr != '\0') numHRI = gs1_encoder_getHRI(ctx, &hri);
		printf("\n    HRI:                    %s\n", *dataStr != '\0' && numHRI == 0 ? "⧚ Not AI-based data ⧚": "");
		for (i = 0; i < numHRI; i++) {
			printf("        %s\n", hri[i]);
		}

		printf("\n\n\nMENU:");
		printf("\n\n 1) Process raw barcode message data, either:");
		printf("\n      * Plain data");
		printf("\n      * Unbracketed AI element string with FNC1 in first position");
		printf("\n      * GS1 Digital Link URI");
		printf("\n 2) Process a bracketed AI element string");
		printf("\n 3) Process barcode scan data (prefixed by AIM Symbology Identifier)");

		printf("\n\n 4) Set 'include data titles in HRI' flag.                  Current value = %s",
					gs1_encoder_getIncludeDataTitlesInHRI(ctx) ? "ON" : "OFF");
		printf("\n 5) Set 'permit unknown AIs' flag.                          Current value = %s",
					gs1_encoder_getPermitUnknownAIs(ctx) ? "ON" : "OFF");
		printf("\n 6) Set 'validate AI mandatory associations' flag.          Current value = %s",
					gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS) ? "ON" : "OFF");
		printf("\n 7) Set 'permit zero-suppressed GTIN in GS1 DL URIs' flag.  Current value = %s",
					gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx) ? "ON" : "OFF");

		printf("\n\n 0) Exit program");

		printf("\n\nMenu selection: ");
		if (gets(inpStr) == NULL)
			return false;

		menuVal = atoi(inpStr);
		switch (menuVal) {
			case 1:
			case 2:
			case 3:
				printf("\nEnter data: ");
				if (gets(inpStr) == NULL)
					continue;
				if (menuVal == 1)
					ret = gs1_encoder_setDataStr(ctx, inpStr);
				else if (menuVal == 2)
					ret = gs1_encoder_setAIdataStr(ctx, inpStr);
				else		// 3
					ret = gs1_encoder_setScanData(ctx, inpStr);
				if (!ret) {
					printf("\n\nERROR message: %s\n", gs1_encoder_getErrMsg(ctx));
					if (*gs1_encoder_getErrMarkup(ctx) != '\0')
						printf("ERROR markup:  %s\n", gs1_encoder_getErrMarkup(ctx));
					continue;
				}
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				printf("\nEnter 0 for OFF or 1 for ON: ");
				if (gets(inpStr) == NULL)
					return false;
				i = atoi(inpStr);
				if (!(i == 0 || i == 1)) {
					printf("OUT OF RANGE. PLEASE ENTER 0 or 1");
					continue;
				}
				if (menuVal == 4)
					ret = gs1_encoder_setIncludeDataTitlesInHRI(ctx, i);
				else if (menuVal == 5)
					ret = gs1_encoder_setPermitUnknownAIs(ctx, i);
				else if (menuVal == 6)
					ret = gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, i);
				else if (menuVal == 7)
					ret = gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, i);
				if (!ret) {
					printf("\n\nERROR: %s\n", gs1_encoder_getErrMsg(ctx));
					continue;
				}
				break;
			case 0:
				return false;
			default:
				printf("OUT OF RANGE. PLEASE ENTER 1-6, 9 or 0.");
		}
	}
	return true;
}

int main(int argc, const char* const argv[]) {

	gs1_encoder* ctx;

	inpStr = malloc((size_t)gs1_encoder_getMaxDataStrLength()+1);
	if (inpStr == NULL) {
		printf("Failed to allocate the input buffer!\n");
		return 1;
	}

	ctx = gs1_encoder_init(NULL);
	if (ctx == NULL) {
		printf("Failed to initialise GS1 Encoders library!\n");
		return 1;
	}

	if (argc == 2 && strcmp(argv[1],"--version") == 0) {
		printf("Application version: " RELEASE "\n");
		printf("Library version: %s\n", gs1_encoder_getVersion());
		goto out;
	}

	printf("\nGS1 Barcode Syntax Engine Console Demo (Built " RELEASE "):");
	printf("\n\nCopyright (c) 2020-2024 GS1 AISBL. License: Apache-2.0");

	while (userInt(ctx));

out:

	gs1_encoder_free(ctx);
	free(inpStr);

	return 0;
}
