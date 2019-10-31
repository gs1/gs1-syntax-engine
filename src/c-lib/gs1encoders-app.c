/**
 * GS1 Syntax Engine
 *
 * @author Copyright (c) 2000-2022 GS1 AISBL.
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
#define gets(i) _gets(i)

static char* _gets(char* in) {

	char* s;

	s = fgets(in, gs1_encoder_getMaxDataStrLength()+1, stdin);
	if (s != NULL) {
		s[strcspn(s, "\r\n")] = 0;
	}
	return s;
}

static bool userInt(gs1_encoder *ctx) {

	bool ret;
	int menuVal, i, numHRI;
	char **hri;

	while (true) {

		printf("\n\n\nCurrent state:\n");

		printf("\n    Barcode message:    %s\n", gs1_encoder_getDataStr(ctx));
		printf("\n    AI element string:  %s", gs1_encoder_getAIdataStr(ctx));
		printf("\n    Digital Link URI:   %s", gs1_encoder_getDLuri(ctx, NULL));
		printf("\n    HRI:\n");
		numHRI = gs1_encoder_getHRI(ctx, &hri);
		for (i = 0; i < numHRI; i++) {
			printf("        %s\n", hri[i]);
		}

		printf("\n\n\nMENU:");
		printf("\n\n 1) Process raw barcode message data, either:");
		printf("\n      * Plain data");
		printf("\n      * Unbracketed AI element string with FNC1 in first position");
		printf("\n      * Digital Link URI");
		printf("\n 2) Process a bracketed AI element string");
		printf("\n 3) Process barcode scan data (prefixed by AIM Symbology Identifier)");

		printf("\n\n 4) Toggle 'include data titles in HRI' flag.  Current value = %s",
					gs1_encoder_getIncludeDataTitlesInHRI(ctx) ? "ON" : "OFF");
		printf("\n 5) Toggle 'permit unknown AIs' flag.          Current value = %s",
					gs1_encoder_getPermitUnknownAIs(ctx) ? "ON" : "OFF");
		printf("\n 6) Toggle 'validate AI associations' flag.    Current value = %s",
					gs1_encoder_getValidateAIassociations(ctx) ? "ON" : "OFF");

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
				if (menuVal == 5)
					ret = gs1_encoder_setPermitUnknownAIs(ctx, i);
				if (menuVal == 6)
					ret = gs1_encoder_setValidateAIassociations(ctx, i);
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

int main(int argc, char *argv[]) {

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

	printf("\nGS1 Syntax Engine Console Demo (Built " RELEASE "):");
	printf("\n\nCopyright (c) 2020-2022 GS1 AISBL. License: Apache-2.0");

	while (userInt(ctx)) {}

out:

	gs1_encoder_free(ctx);
	free(inpStr);

	return 0;
}
