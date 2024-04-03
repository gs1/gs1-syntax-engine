/**
 * GS1 Syntax Engine
 *
 * @author Copyright (c) 2021-2024 GS1 AISBL.
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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax/gs1syntaxdictionary.h"
#include "enc-private.h"
#include "gs1encoders.h"
#include "dl.h"


typedef enum {
	aiMode_AI,
	aiMode_NON_AI
} aiMode_t;


struct symIdEntry {
	char symId[2];
	aiMode_t aiMode;
	gs1_encoder_symbologies_t sym;
};


#define SYM(i1, i2, a, s) {		\
	.symId = { i1, i2 },		\
	.aiMode = aiMode_##a,		\
	.sym = s,			\
}


/*
 *  First matching instance in a lookup is the default
 *
 */
static const struct symIdEntry symIdTable[] = {
	SYM( 'C','1', AI,     gs1_encoder_sGS1_128_CCA        ),
	SYM( 'C','1', AI,     gs1_encoder_sGS1_128_CCC        ),
	SYM( 'E','0', NON_AI, gs1_encoder_sEAN13              ),
	SYM( 'E','0', AI,     gs1_encoder_sEAN13              ),
	SYM( 'E','0', NON_AI, gs1_encoder_sUPCA               ),
	SYM( 'E','0', AI,     gs1_encoder_sUPCA               ),
	SYM( 'E','0', NON_AI, gs1_encoder_sUPCE               ),
	SYM( 'E','0', AI,     gs1_encoder_sUPCE               ),
	SYM( 'E','4', NON_AI, gs1_encoder_sEAN8               ),
	SYM( 'E','4', AI,     gs1_encoder_sEAN8               ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarExpanded    ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarOmni        ),
	SYM( 'e','0', NON_AI, gs1_encoder_sDataBarOmni        ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarTruncated   ),
	SYM( 'e','0', NON_AI, gs1_encoder_sDataBarTruncated   ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarStacked     ),
	SYM( 'e','0', NON_AI, gs1_encoder_sDataBarStacked     ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarStackedOmni ),
	SYM( 'e','0', NON_AI, gs1_encoder_sDataBarStackedOmni ),
	SYM( 'e','0', AI,     gs1_encoder_sDataBarLimited     ),
	SYM( 'e','0', NON_AI, gs1_encoder_sDataBarLimited     ),
	// e0 is also shared with GS1-128 with CC
	SYM( 'd','1', NON_AI, gs1_encoder_sDM                 ),
	SYM( 'd','2', AI,     gs1_encoder_sDM                 ),
	SYM( 'Q','1', NON_AI, gs1_encoder_sQR                 ),
	SYM( 'Q','3', AI,     gs1_encoder_sQR                 ),
};

#undef SYM

#define CC_SYM_ID "]e0"


static const char* lookupSymId(gs1_encoder* const ctx) {

	size_t i;
	const char *symId = NULL;

	for (i = 0; i < SIZEOF_ARRAY(symIdTable); i++) {
		const struct symIdEntry* const entry = &symIdTable[i];
		if (entry->sym != ctx->sym ||
		    entry->aiMode != (*ctx->dataStr == '^' ? aiMode_AI : aiMode_NON_AI))
			continue;
		symId = entry->symId;
		break;
	}

	assert(symId);

	return symId;

}

static void lookupSymAndModeBySymId(const char* const symId, gs1_encoder_symbologies_t* const sym, aiMode_t* const aiMode) {

	size_t i;

	*sym = gs1_encoder_sNONE;
	*aiMode = aiMode_NON_AI;

	for (i = 0; i < SIZEOF_ARRAY(symIdTable); i++) {
		const struct symIdEntry* const entry = &symIdTable[i];
		if (memcmp(symId, entry->symId, 2) != 0)
			continue;
		*sym = entry->sym;
		*aiMode = entry->aiMode;
		break;
	}

}


static void scancat(char* const out, const char* const in) {

	const char *p = in;
	char *q = out;

	while (*q)
		q++;						// Got to end of output

	if (*p == '^') {					// GS1 mode

		// Skip the leading FNC1 since we are following a symbology identifier
		while (*++p)
			*q++ = (*p == '^') ? '\x1D' : *p;	// Convert encoded FNC1 to GS

		if (*(p - 1) == '^')				// Strip any trailing FNC1
			q--;

	}
	else {

		// Unescape leading sequence "\\...^" -> "\...^"
		const char *r = p;
		while (*r == '\\')
			r++;
		if (*r == '^')
			p++;

		while (*p)
			*q++ = *p++;
	}
	*q = '\0';

}


static bool validateParity(uint8_t *str) {

	int weight;
	int parity = 0;

	assert(*str);

	weight = strlen((char*)str) % 2 == 0 ? 3 : 1;
	while (*(str+1)) {
		parity += weight * (*str++ - '0');
		weight = 4 - weight;
	}
	parity = (10 - parity%10) % 10;

	if (parity + '0' == *str) return true;

	*str = (uint8_t)(parity + '0');		// Recalculate
	return false;

}


static bool checkAndNormalisePrimaryData(gs1_encoder* const ctx, const char *dataStr, char* const primaryStr, int length) {

	if (strlen(dataStr) != (size_t)(ctx->addCheckDigit ? length-1 : length)) {
		if (ctx->addCheckDigit)
			snprintf(ctx->errMsg, sizeof(ctx->errMsg), "Primary data must be %d digits without check digit", length - 1);
		else
			snprintf(ctx->errMsg, sizeof(ctx->errMsg), "Primary data must be %d digits", length);
		ctx->errFlag = true;
		return false;
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		return false;
	}

	strcpy(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		return false;
	}

	return true;

}


char* gs1_generateScanData(gs1_encoder* const ctx) {

	char* cc = NULL;
	char primaryStr[15];
	const char *pad;
	const char *dataStr;
	int length, aizeros;
	char* ret;

	assert(ctx);

	*ctx->outStr = '\0';

	if ((cc = strchr(ctx->dataStr, '|')) != NULL)		// Delimit end of linear data
		*cc++ = '\0';

	switch (ctx->sym) {

	case gs1_encoder_sQR:
	case gs1_encoder_sDM:

		// QR: "]Q1" for plain data; "]Q3" for GS1 data
		// DM: "]d1" for plain data; "]d2" for GS1 data

		// If plain data then put original faux CC delimiter back
		if (*ctx->dataStr != '^' && cc)
			*--cc = '|';

		strcat(ctx->outStr, "]");
		strncat(ctx->outStr, lookupSymId(ctx), 2);
		scancat(ctx->outStr, ctx->dataStr);
		break;

	case gs1_encoder_sGS1_128_CCA:
	case gs1_encoder_sGS1_128_CCC:

		if (!cc) {
			// "]C1" for linear-only GS1-128
			if (*ctx->dataStr != '^')
				goto fail;
			strcat(ctx->outStr, "]");
			strncat(ctx->outStr, lookupSymId(ctx), 2);
			scancat(ctx->outStr, ctx->dataStr);
			break;
		}

		/* FALLTHROUGH */

	case gs1_encoder_sDataBarExpanded:  // And GS1-128 Composite

		// "]e0" followed by concatenated AI data from linear and CC
		if (*ctx->dataStr != '^')
			goto fail;
		strcat(ctx->outStr, CC_SYM_ID);
		scancat(ctx->outStr, ctx->dataStr);

		if (cc) {

			bool lastAIfnc1 = false;
			int i;

			if (*cc != '^')
				goto fail;

			// Append GS if last AI of linear component isn't fixed-length
			for (i = 0; i < ctx->numAIs && ctx->aiData[i].aiEntry; i++)
				lastAIfnc1 = ctx->aiData[i].aiEntry->fnc1;
			if (lastAIfnc1)
				strcat(ctx->outStr, "\x1D");

			scancat(ctx->outStr, cc);

		}

		break;

	case gs1_encoder_sDataBarOmni:
	case gs1_encoder_sDataBarTruncated:
	case gs1_encoder_sDataBarStacked:
	case gs1_encoder_sDataBarStackedOmni:
	case gs1_encoder_sDataBarLimited:

		// "]e0" followed by concatenated AI data from linear and CC

		dataStr = ctx->dataStr;
		if (strlen(dataStr) >= 3 && strncmp(dataStr, "^01", 3) == 0)
			dataStr += 3;

		if (!checkAndNormalisePrimaryData(ctx, dataStr, primaryStr, 14))
			goto fail;

		// GS1 DataBar Limited is restricted to low-valued inputs
		if (ctx->sym == gs1_encoder_sDataBarLimited) {
			if (atof((char*)primaryStr) > 19999999999999.) {
				strcpy(ctx->errMsg, "Primary data item value is too large");
				ctx->errFlag = true;
				goto fail;
			}
		}

		strcat(ctx->outStr, "]");
		strncat(ctx->outStr, lookupSymId(ctx), 2);
		strcat(ctx->outStr, "01");		// Convert to AI (01)
		scancat(ctx->outStr, primaryStr);

		if (cc) {
			if (*cc != '^')
				goto fail;
			scancat(ctx->outStr, cc);
		}

		break;

	case gs1_encoder_sUPCA:
	case gs1_encoder_sUPCE:
	case gs1_encoder_sEAN13:
	case gs1_encoder_sEAN8:

		// Primary is "]E0" then 13 digits (or "]E4" then 8 digits for EAN-8)
		// CC is new message beginning "]e0"

		if (ctx->sym == gs1_encoder_sEAN13) {
			length = 13;
			pad = "";
		}
		else if (ctx->sym == gs1_encoder_sEAN8) {
			length = 8;
			pad = "";
		}
		else {  // UPC-A or UPC-E
			length = 12;
			pad = "0";		// As normalised to 12 digits
		}

		// If AI data beginning (01) then skip leading zeros of the GTIN-14
		dataStr = ctx->dataStr;
		aizeros = 17 - length;
		if (strlen(dataStr) >= (size_t)aizeros && strncmp(dataStr, "^01000000", (size_t)aizeros) == 0)
			dataStr += aizeros;

		if (!checkAndNormalisePrimaryData(ctx, dataStr, primaryStr, length))
			goto fail;

		strcat(ctx->outStr, "]");
		strncat(ctx->outStr, lookupSymId(ctx), 2);
		strcat(ctx->outStr, pad);
		scancat(ctx->outStr, primaryStr);
		if (cc) {
			if (*cc != '^')
				goto fail;
			strcat(ctx->outStr, "|" CC_SYM_ID);		// "|" means start of new message
			scancat(ctx->outStr, cc);
		}
		break;

	case gs1_encoder_sNONE:
	case gs1_encoder_sNUMSYMS:
		goto fail;

	}

	ret = ctx->outStr;

out:

	if (cc) {
//		*(cc-1) = '|';					// Put original separator back
		ctx->dataStr[cc - ctx->dataStr - 1] = '|';	// Ugly hack generates same code as above to satiate GCC 11
	}
	return ret;

fail:

	ret = NULL;
	goto out;

}


bool gs1_processScanData(gs1_encoder* const ctx, const char* scanData) {

	gs1_encoder_symbologies_t sym;
	aiMode_t aiMode;
	char *p;
	const char *q;

	assert(ctx);
	assert(scanData);

	ctx->sym = gs1_encoder_sNONE;
	*ctx->dataStr = '\0';
	ctx->numAIs = 0;

	*ctx->errMsg = '\0';
	ctx->errFlag = false;
	ctx->linterErr = GS1_LINTER_OK;
	*ctx->linterErrMarkup = '\0';

	if (*scanData != ']' || strlen(scanData) < 3) {
		strcpy(ctx->errMsg, "Missing symbology identifier");
		goto fail;
	}

	lookupSymAndModeBySymId(scanData + 1, &sym, &aiMode);

	if (sym == gs1_encoder_sNONE) {
		strcpy(ctx->errMsg, "Unsupported symbology identifier");
		goto fail;
	}

	scanData += 3;
	ctx->sym = sym;
	p = ctx->dataStr;

	if (sym == gs1_encoder_sEAN13 || sym == gs1_encoder_sEAN8) {

		size_t primaryLen = (sym == gs1_encoder_sEAN13) ? 13 : 8;
		const char *cc = NULL;

		if (strlen(scanData) < primaryLen) {
			strcpy(ctx->errMsg, "Primary scan data is too short");
			goto fail;
		}

		if (strlen(scanData) >= primaryLen + sizeof(CC_SYM_ID) &&
		    strncmp(scanData + primaryLen, "|" CC_SYM_ID, sizeof(CC_SYM_ID)) == 0) {
			cc = scanData + primaryLen + sizeof(CC_SYM_ID);
		} else if (strlen(scanData) > primaryLen) {
			strcpy(ctx->errMsg, "Primary message is too long");
			goto fail;
		}

		*p = '\0';
		strncat(p, scanData, primaryLen);

		if (!gs1_allDigits((uint8_t*)p, 0)) {
			strcpy(ctx->errMsg, "Primary message number only contain digits");
			goto fail;
		}

		if (!validateParity((uint8_t*)p)) {
			strcpy(ctx->errMsg, "Primary message check digit is incorrect");
			goto fail;
		}

		if (!cc)
			return true;

		// Process CC as AI data
		p += primaryLen;
		*p++ = '|';
		scanData = cc;
		aiMode = aiMode_AI;

	}

	if (aiMode == aiMode_AI) {

		q = p;
		*p++ = '^';

		// Forbid data "^" characters at this stage so we don't conflate with FNC1
		if (strchr(scanData, '^') != NULL) {
			strcpy(ctx->errMsg, "Scan data contains illegal ^ character");
			goto fail;
		}

		strcpy(p, scanData);
		while (*p) {
			if (*p == 0x1D)		// GS character represents FNC1
				*p = '^';
			p++;
		}
		if (!gs1_processAIdata(ctx, q, true))	// Validate AI data and extract AIs
			goto fail;

		return true;

	}

	// From here plain data

	// Disambiguate from GS1 data: "^" -> "\^" ; "\^" -> "\\^", etc
	q = scanData;
	while (*q == '\\')
		q++;
	if (*q == '^')
		*p++ = '\\';
	strcpy(p, scanData);

	// If a GS1 Digital Link URI is given then process it immediately
	if ((strlen(ctx->dataStr) >= 8 && strncmp(ctx->dataStr, "https://", 8) == 0) ||
	    (strlen(ctx->dataStr) >= 7 && strncmp(ctx->dataStr, "http://",  7) == 0)) {
		// We extract AIs with the element string stored in dlAIbuffer
		if (!gs1_parseDLuri(ctx, ctx->dataStr, ctx->dlAIbuffer))
			goto fail;
	}

	return true;

fail:

	*ctx->dataStr = '\0';
	ctx->sym = gs1_encoder_sNONE;
	ctx->errFlag = true;

	return false;

}



#ifdef UNIT_TESTS

#define TEST_NO_MAIN
#include "acutest.h"


void test_scandata_validateParity(void) {

	char good_gtin14[] = "24012345678905";
	char bad_gtin14[]  = "24012345678909";
	char good_gtin13[] = "2112233789657";
	char bad_gtin13[]  = "2112233789658";
	char good_gtin12[] = "416000336108";
	char bad_gtin12[]  = "416000336107";
	char good_gtin8[]  = "02345680";
	char bad_gtin8[]   = "02345689";

	TEST_CHECK(validateParity((uint8_t*)good_gtin14));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin14));
	TEST_CHECK(bad_gtin14[13] == '5');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin13));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin13));
	TEST_CHECK(bad_gtin13[12] == '7');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin12));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin12));
	TEST_CHECK(bad_gtin12[11] == '8');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin8));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin8));
	TEST_CHECK(bad_gtin8[7] == '0');		// Recomputed

}


static void do_test_testGenerateScanData(gs1_encoder* const ctx, const char* const name, const gs1_encoder_symbologies_t sym, const char* const dataStr, const char* const expect) {

	char *out;
	char casename[256];

	snprintf(casename, sizeof(casename), "%s: %s", name, dataStr);
	TEST_CASE(casename);

	TEST_ASSERT(gs1_encoder_setSym(ctx, sym));
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, dataStr));
	out = gs1_generateScanData(ctx);
	if (!expect) {
		TEST_CHECK(out == NULL);
		TEST_MSG("Given: %s; Got: %s; Expected: NULL", dataStr, out);
		return;
	}
	TEST_ASSERT(out != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, expect) == 0);
	TEST_MSG("Given: %s; Got: %s; Expected: %s", dataStr, out, expect);

}


void test_scandata_generateScanData(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);

#define test_testGenerateScanData(n, d, e) do {					\
	do_test_testGenerateScanData(ctx, #n, gs1_encoder_s##n, d, e);			\
} while (0)

	test_testGenerateScanData(NONE, "", NULL);
	test_testGenerateScanData(NONE, "TESTING", NULL);

	/* QR */
	test_testGenerateScanData(QR, "TESTING", "]Q1TESTING");
	test_testGenerateScanData(QR, "\\^TESTING", "]Q1^TESTING");		// Escaped data "^" character
	test_testGenerateScanData(QR, "\\\\^TESTING", "]Q1\\^TESTING");		// Escaped data "\^" characters
	test_testGenerateScanData(QR, "^011231231231233310ABC123^99TESTING",
		"]Q3011231231231233310ABC123" "\x1D" "99TESTING");

	/* DM */
	test_testGenerateScanData(DM, "TESTING", "]d1TESTING");
	test_testGenerateScanData(DM, "\\^TESTING", "]d1^TESTING");		// Escaped data "^" character
	test_testGenerateScanData(DM, "\\\\^TESTING", "]d1\\^TESTING");		// Escaped data "\^" characters
	test_testGenerateScanData(DM, "^011231231231233310ABC123^99TESTING",
		"]d2011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(DM, "^011231231231233310ABC123^99TESTING^",
		"]d2011231231231233310ABC123" "\x1D" "99TESTING");		// Trailing FNC1 should be stripped

	/* DataBar Expanded */
	test_testGenerateScanData(DataBarExpanded, "^011231231231233310ABC123^99TESTING",
		"]e0011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(DataBarExpanded, 				// ... Variable-length AI | Composite
		"^011231231231233310ABC123^99TESTING|^98COMPOSITE^97XYZ",
		"]e0011231231231233310ABC123" "\x1D" "99TESTING" "\x1D" "98COMPOSITE" "\x1D" "97XYZ");
	test_testGenerateScanData(DataBarExpanded,				// ... Fixed-Length AI | Composite
		"^011231231231233310ABC123^11991225|^98COMPOSITE^97XYZ",
		"]e0011231231231233310ABC123" "\x1D" "1199122598COMPOSITE" "\x1D" "97XYZ");

	test_testGenerateScanData(GS1_128_CCA, "^011231231231233310ABC123^99TESTING",
		"]C1011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(GS1_128_CCA,					// Composite uses ]e0
		"^011231231231233310ABC123^99TESTING|^98COMPOSITE^97XYZ",
		"]e0011231231231233310ABC123" "\x1D" "99TESTING" "\x1D" "98COMPOSITE" "\x1D" "97XYZ");
	test_testGenerateScanData(GS1_128_CCC,					// Composite uses ]e0
		"^011231231231233310ABC123^99TESTING|^98COMPOSITE^97XYZ",
		"]e0011231231231233310ABC123" "\x1D" "99TESTING" "\x1D" "98COMPOSITE" "\x1D" "97XYZ");

	/* DataBar OmniDirectional */
	test_testGenerateScanData(DataBarOmni, "^0124012345678905|^99COMPOSITE^98XYZ",
		"]e0012401234567890599COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(DataBarOmni, "24012345678905|^99COMPOSITE^98XYZ",
		"]e0012401234567890599COMPOSITE" "\x1D" "98XYZ");

	/* DataBar Limited */
	test_testGenerateScanData(DataBarLimited, "^0115012345678907|^99COMPOSITE^98XYZ",
		"]e0011501234567890799COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(DataBarLimited, "15012345678907|^99COMPOSITE^98XYZ",
		"]e0011501234567890799COMPOSITE" "\x1D" "98XYZ");

	/* UPC-A */
	test_testGenerateScanData(UPCA, "^0100416000336108|^99COMPOSITE^98XYZ",
		"]E00416000336108|]e099COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(UPCA, "416000336108|^99COMPOSITE^98XYZ",
		"]E00416000336108|]e099COMPOSITE" "\x1D" "98XYZ");

	/* UPC-E */
	test_testGenerateScanData(UPCE, "^0100001234000057|^99COMPOSITE^98XYZ",
		"]E00001234000057|]e099COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(UPCE, "001234000057|^99COMPOSITE^98XYZ",
		"]E00001234000057|]e099COMPOSITE" "\x1D" "98XYZ");

	/* EAN-13 */
	test_testGenerateScanData(EAN13, "^0102112345678900|^99COMPOSITE^98XYZ",
		"]E02112345678900|]e099COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(EAN13, "2112345678900|^99COMPOSITE^98XYZ",
		"]E02112345678900|]e099COMPOSITE" "\x1D" "98XYZ");

	/* EAN-8 */
	test_testGenerateScanData(EAN8, "^0100000002345673|^99COMPOSITE^98XYZ",
		"]E402345673|]e099COMPOSITE" "\x1D" "98XYZ");
	test_testGenerateScanData(EAN8, "02345673|^99COMPOSITE^98XYZ",
		"]E402345673|]e099COMPOSITE" "\x1D" "98XYZ");

#undef test_testGenerateScanData

	gs1_encoder_free(ctx);

}


static void do_test_testProcessScanData(gs1_encoder* const ctx, const bool should_succeed, const char* const scanData,
		const char* const expectSymName, const gs1_encoder_symbologies_t expectSym, const char* const expectDataStr) {

	char casename[256];

	snprintf(casename, sizeof(casename), "%s", scanData);
	TEST_CASE(casename);

	TEST_CHECK(gs1_processScanData(ctx, scanData) ^ (!should_succeed));
	TEST_MSG("Error message: %s", ctx->errMsg);
	TEST_CHECK(ctx->sym == expectSym);
	TEST_MSG("Got: %d; Expected: %d (%s)", ctx->sym, expectSym, expectSymName);
	TEST_CHECK(strcmp(ctx->dataStr, expectDataStr) == 0);
	TEST_MSG("Got: %s; Expected: %s", ctx->dataStr, expectDataStr);

}


void test_scandata_processScanData(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);											// Satisfy analyzer

#define test_testProcessScanData(ss, sd, s, d) do {				\
	do_test_testProcessScanData(ctx, ss, sd, #s, gs1_encoder_s##s, d);	\
} while (0)

	test_testProcessScanData(false, "", NONE, "");			// No data
	test_testProcessScanData(false, "ABC", NONE, "");		// No symbology identifier
	test_testProcessScanData(false, "]", NONE, "");			// Short
	test_testProcessScanData(false, "]X", NONE, "");		// Short
	test_testProcessScanData(false, "]XX", NONE, "");		// Unknown symbology identifier

	test_testProcessScanData(false, "]e0", NONE, "");		// Empty GS1 data

	/* QR */
	test_testProcessScanData(true, "]Q1", QR, "");
	test_testProcessScanData(true, "]Q1TESTING", QR, "TESTING");
	test_testProcessScanData(true, "]Q1^TESTING", QR, "\\^TESTING");
	test_testProcessScanData(true, "]Q1\\^TESTING", QR, "\\\\^TESTING");
	test_testProcessScanData(false, "]Q3", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]Q3011231231231233310ABC123" "\x1D" "99TESTING",
		QR, "^011231231231233310ABC123^99TESTING");

	/* QR Code with GS1 Digital Link URI */
	test_testProcessScanData(true, "]Q1https://example.com/01/12312312312333?99=TEST",
		QR, "https://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "^011231231231233399TEST") == 0);	// Check AI extraction

	/* DM */
	test_testProcessScanData(true, "]d1", DM, "");
	test_testProcessScanData(true, "]d1TESTING", DM, "TESTING");
	test_testProcessScanData(true, "]d1^TESTING", DM, "\\^TESTING");
	test_testProcessScanData(true, "]d1\\^TESTING", DM, "\\\\^TESTING");
	test_testProcessScanData(false, "]d2", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]d2011231231231233310ABC123" "\x1D" "99TESTING",
		DM, "^011231231231233310ABC123^99TESTING");

	/* DM with GS1 Digital Link URI */
	test_testProcessScanData(true, "]d1https://example.com/01/12312312312333?99=TEST",
		DM, "https://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "^011231231231233399TEST") == 0);	// Check AI extraction

	/* DataBar Expanded, shared with all DataBar family and UCC-128 Composite */
	test_testProcessScanData(false, "]e0", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]e0011231231231233310ABC123" "\x1D" "99TESTING",
		DataBarExpanded, "^011231231231233310ABC123^99TESTING");
	test_testProcessScanData(true, "]e0011231231231233310ABC123" "\x1D" "99TESTING" "\x1D" "98XYZ",
		DataBarExpanded, "^011231231231233310ABC123^99TESTING^98XYZ");
	test_testProcessScanData(true, "]e0011231231231233310ABC123" "\x1D" "1199122598TESTING" "\x1D" "97XYZ",
		DataBarExpanded, "^011231231231233310ABC123^1199122598TESTING^97XYZ");

	/* GS1-128 linear-only; composite is ]e0 */
	test_testProcessScanData(false, "]C1", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]C1011231231231233310ABC123" "\x1D" "99TESTING",
		GS1_128_CCA, "^011231231231233310ABC123^99TESTING");

	/* EAN/UPC, except EAN-8 */
	test_testProcessScanData(false, "]E0", NONE, "");
	test_testProcessScanData(false, "]E0123456789012", NONE, "");	// Short
	test_testProcessScanData(false, "]E012345678901234", NONE, "");	// Long
	test_testProcessScanData(false, "]E01234ABC890123", NONE, "");	// Non-numeric
	test_testProcessScanData(false, "]E02112345678901", NONE, "");	// Bad check digit
	test_testProcessScanData(true, "]E02112345678900",
		EAN13, "2112345678900");
	test_testProcessScanData(true, "]E02112345678900|]e099COMPOSITE" "\x1D" "98XYZ",
		EAN13, "2112345678900|^99COMPOSITE^98XYZ");

	/* EAN-8 */
	test_testProcessScanData(false, "]E4", NONE, "");
	test_testProcessScanData(false, "]E41234567", NONE, "");	// Short
	test_testProcessScanData(false, "]E4123456789", NONE, "");	// Long
	test_testProcessScanData(false, "]E412ABC678", NONE, "");	// Non-numeric
	test_testProcessScanData(false, "]E402345674", NONE, "");	// Bad check digit
	test_testProcessScanData(true, "]E402345673",
		EAN8, "02345673");
	test_testProcessScanData(true, "]E402345673|]e099COMPOSITE" "\x1D" "98XYZ",
		EAN8, "02345673|^99COMPOSITE^98XYZ");

#undef test_testProcessScanData

	gs1_encoder_free(ctx);

}


#endif  /* UNIT_TESTS */
