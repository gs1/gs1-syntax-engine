/**
 * GS1 Syntax Engine
 *
 * @author Copyright (c) 2021-2022 GS1 AISBL.
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
#include "ai.h"
#include "dl.h"


#define SYM(i, a, s) {				\
	.identifier = i,			\
	.aiMode = a,				\
	.defaultSym = s,			\
}


struct symIdEntry {
	char *identifier;
	bool aiMode;
	enum gs1_encoder_symbologies defaultSym;
};


#define AI true
#define NON_AI false

static const struct symIdEntry symIdTable[] = {
	SYM( "]C1", AI,     gs1_encoder_sGS1_128_CCA ),
	SYM( "]E0", NON_AI, gs1_encoder_sEAN13 ),
	SYM( "]E4", NON_AI, gs1_encoder_sEAN8 ),
	SYM( "]e0", AI,     gs1_encoder_sDataBarExpanded ),	// Shared with GS1-128 CC
	SYM( "]d1", NON_AI, gs1_encoder_sDM ),
	SYM( "]d2", AI,     gs1_encoder_sDM ),
	SYM( "]Q1", NON_AI, gs1_encoder_sQR ),
	SYM( "]Q3", AI,     gs1_encoder_sQR ),
};


static void scancat(char* out, const char* in) {

	const char *p, *r;
	char *q;

	p = in;
	q = out;

	while (*q)
		q++;					// Got to end of output

	if (*p == '^') {				// GS1 mode
		p++;					// Skip the leading FNC1 since we are following a symbology identifier
		while (*p) {
			if (*p == '^')			// Convert encoded FNC1 to GS
				*q++ = '\x1D';
			else
				*q++ = *p;
			p++;
		}
		if (*(p - 1) == '^')			// Strip any trailing FNC1
			q--;
	}
	else {

		// Unescape leading sequence "\\...^" -> "\...^"
		r = p;
		while (*r == '\\')
			r++;
		if (*r == '^')
			p++;

		while (*p)
			*q++ = *p++;
	}
	*q = '\0';

}


static bool gs1_normaliseEAN13(gs1_encoder *ctx, const char *dataStr, char *primaryStr) {

	unsigned int digits = ctx->sym == gs1_encoder_sEAN13 ? 13 : 12;

	if (strlen(dataStr) >= 17-(size_t)digits && strncmp(dataStr, "^0100", 17-(size_t)digits) == 0)
		dataStr += 17-digits;

	if (!ctx->addCheckDigit) {
		if (strlen(dataStr) != digits) {
			sprintf(ctx->errMsg, "Primary data must be %d digits", digits);
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}
	else {
		if (strlen(dataStr) != (size_t)digits-1) {
			sprintf(ctx->errMsg, "Primary data must be %d digits without check digit", digits-1);
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	primaryStr[0] = ctx->sym == gs1_encoder_sEAN13 ? '\0' : '0';  // Convert GTIN-12 to GTIN-13 if UPC-A
	primaryStr[1] = '\0';
	strcat(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!gs1_validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	return true;

}


static bool gs1_normaliseEAN8(gs1_encoder *ctx, const char* dataStr, char* primaryStr) {

	if (strlen(dataStr) >= 9 && strncmp(dataStr, "^01000000", 9) == 0)
		dataStr += 9;

	if (!ctx->addCheckDigit) {
		if (strlen(dataStr) != 8) {
			strcpy(ctx->errMsg, "Primary data must be 8 digits");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}
	else {
		if (strlen(dataStr) != 7) {
			strcpy(ctx->errMsg, "Primary data must be 7 digits without check digit");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	strcpy(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!gs1_validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	return true;

}


static bool gs1_normaliseUPCE(gs1_encoder *ctx, const char *dataStr, char *primaryStr) {

	if (strlen(dataStr) >= 5 && strncmp(dataStr, "^0100", 5) == 0)
		dataStr += 5;

	if (!ctx->addCheckDigit) {
		if (strlen(dataStr) != 12) {
			strcpy(ctx->errMsg, "Primary data must be 12 digits");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}
	else {
		if (strlen(dataStr) != 11) {
			strcpy(ctx->errMsg, "Primary data must be 11 digits without check digit");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	strcpy(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!gs1_validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	return true;

}


static bool gs1_normaliseRSS14(gs1_encoder *ctx, const char *dataStr, char *primaryStr) {

	if (strlen(dataStr) >= 3 && strncmp(dataStr, "^01", 3) == 0)
		dataStr += 3;

	if (!ctx->addCheckDigit) {
		if (strlen(dataStr) != 14) {
			strcpy(ctx->errMsg, "Primary data must be a GTIN-14");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}
	else {
		if (strlen(dataStr) != 13) {
			strcpy(ctx->errMsg, "Primary data must be a GTIN-14 without check digit");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	strcpy(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!gs1_validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	return true;

}


static bool gs1_normaliseRSSLim(gs1_encoder *ctx, const char *dataStr, char *primaryStr) {

	if (strlen(dataStr) >= 3 && strncmp(dataStr, "^01", 3) == 0)
	dataStr += 3;

	if (!ctx->addCheckDigit) {
		if (strlen(dataStr) != 14) {
			strcpy(ctx->errMsg, "Primary data must be 14 digits");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}
	else {
		if (strlen(dataStr) != 13) {
			strcpy(ctx->errMsg, "Primary data must be 13 digits without check digit");
			ctx->errFlag = true;
			*primaryStr = '\0';
			return false;
		}
	}

	if (!gs1_allDigits((uint8_t*)dataStr, 0)) {
		strcpy(ctx->errMsg, "Primary data must be all digits");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	strcpy(primaryStr, dataStr);

	if (ctx->addCheckDigit)
		strcat(primaryStr, "-");

	if (!gs1_validateParity((uint8_t*)primaryStr) && !ctx->addCheckDigit) {
		strcpy(ctx->errMsg, "Primary data check digit is incorrect");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	if (atof((char*)primaryStr) > 19999999999999.) {
		strcpy(ctx->errMsg, "Primary data item value is too large");
		ctx->errFlag = true;
		*primaryStr = '\0';
		return false;
	}

	return true;

}


char* gs1_generateScanData(gs1_encoder* ctx) {

	char* cc = NULL;
	int i;
	bool lastAIfnc1;
	char *prefix;
	char primaryStr[15];
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

		if (*ctx->dataStr == '^') {
			strcat(ctx->outStr, ctx->sym == gs1_encoder_sQR ? "]Q3" : "]d2");
		} else {
			strcat(ctx->outStr, ctx->sym == gs1_encoder_sQR ? "]Q1" : "]d1");
			if (cc)
				*--cc = '|';	// Plain data so put original character back
		}
		scancat(ctx->outStr, ctx->dataStr);
		break;

	case gs1_encoder_sGS1_128_CCA:
	case gs1_encoder_sGS1_128_CCC:

		if (!cc) {
			// "]C1" for linear-only GS1-128
			if (*ctx->dataStr != '^')
				goto fail;
			strcat(ctx->outStr, "]C1");
			scancat(ctx->outStr, ctx->dataStr);
			break;
		}

		/* FALLTHROUGH */  // For GS1-128 Composite

	case gs1_encoder_sDataBarExpanded:

		// "]e0" followed by concatenated AI data from linear and CC
		if (*ctx->dataStr != '^')
			goto fail;
		strcat(ctx->outStr, "]e0");
		scancat(ctx->outStr, ctx->dataStr);

		if (cc) {
			if (*cc != '^')
				goto fail;

			// Append GS if last AI of linear component isn't fixed-length
			lastAIfnc1 = false;
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

		// Normalise input to 14 digits
		if (ctx->sym == gs1_encoder_sDataBarLimited)
			gs1_normaliseRSSLim(ctx, ctx->dataStr, primaryStr);
		else
			gs1_normaliseRSS14(ctx, ctx->dataStr, primaryStr);

		if (*primaryStr == '\0')
			goto fail;

		strcat(ctx->outStr, "]e001");		// Convert to AI (01)
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

		// Normalise input
		if (ctx->sym == gs1_encoder_sEAN8) {
			gs1_normaliseEAN8(ctx, ctx->dataStr, primaryStr);
			prefix = "]E4";
		}
		else if (ctx->sym == gs1_encoder_sUPCE) {
			gs1_normaliseUPCE(ctx, ctx->dataStr, primaryStr);
			prefix = "]E00";	// UPCE is normalised to 12 digits
		}
		else {	// EAN13 and UPC-A
			gs1_normaliseEAN13(ctx, ctx->dataStr, primaryStr);
			prefix = "]E0";
		}

		if (*primaryStr == '\0')
			goto fail;

		strcat(ctx->outStr, prefix);
		scancat(ctx->outStr, primaryStr);
		if (cc) {
			if (*cc != '^')
				goto fail;
			strcat(ctx->outStr, "|]e0");		// "|" means start of new message
			scancat(ctx->outStr, cc);
		}
		break;

	}

	ret = ctx->outStr;

out:

	if (cc)
		*(cc - 1) = '|';			// Put original separator back
	return ret;

fail:

	ret = NULL;
	goto out;

}


bool gs1_processScanData(gs1_encoder* ctx, const char* scanData) {

	size_t i;
	bool aiMode = false;
	enum gs1_encoder_symbologies sym = gs1_encoder_sNONE;
	const struct symIdEntry *entry;
	const size_t symIdTable_len = SIZEOF_ARRAY(symIdTable);
	char *p;
	const char *q, *cc = NULL;
	size_t primaryLen;

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

	for (i = 0; i < symIdTable_len; i++) {
		entry = &symIdTable[i];
		if (strncmp(scanData, entry->identifier, 3) != 0)
			continue;
		aiMode = entry->aiMode;
		sym = entry->defaultSym;
		break;
	}

	if (sym == gs1_encoder_sNONE) {
		strcpy(ctx->errMsg, "Unsupported symbology identifier");
		goto fail;
	}

	scanData += 3;
	ctx->sym = sym;
	p = ctx->dataStr;

	if (sym == gs1_encoder_sEAN13 || sym == gs1_encoder_sEAN8) {

		primaryLen = (sym == gs1_encoder_sEAN13) ? 13 : 8;

		if (strlen(scanData) < primaryLen) {
			strcpy(ctx->errMsg, "Primary scan data is too short");
			goto fail;
		}

		if (strlen(scanData) >= primaryLen + 4 &&
		    strncmp(scanData + primaryLen, "|]e0", 4) == 0) {
			cc = scanData + primaryLen + 4;
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

		if (!gs1_validateParity((uint8_t*)p)) {
			strcpy(ctx->errMsg, "Primary message check digit is incorrect");
			goto fail;
		}

		if (!cc)
			return true;

		// Process CC as AI data
		p += primaryLen;
		*p++ = '|';
		scanData = cc;
		aiMode = true;

	}

	if (aiMode) {

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

	// If a Digital Link URI is given then process it immediately
	if ((strlen(ctx->dataStr) >= 8 && strncmp(ctx->dataStr, "https://", 8) == 0) || // Digital Link URI
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


#define test_testGenerateScanData(n, d, e) do {					\
	do_test_testGenerateScanData(ctx, #n, gs1_encoder_s##n, d, e);			\
} while (0)

static void do_test_testGenerateScanData(gs1_encoder* ctx, const char* name, const int sym, const char* dataStr, const char* expect) {

	char *out;
	char casename[256];

	sprintf(casename, "%s: %s", name, dataStr);
	TEST_CASE(casename);

	TEST_ASSERT(gs1_encoder_setSym(ctx, sym));
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, dataStr));
	TEST_ASSERT((out = gs1_generateScanData(ctx)) != NULL);
	TEST_CHECK(strcmp(out, expect) == 0);
	TEST_MSG("Given: %s; Got: %s; Expected: %s", dataStr, out, expect);

}


void test_scandata_generateScanData(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);

	test_testGenerateScanData(NONE, "", "");
	test_testGenerateScanData(NONE, "TESTING", "");

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

	/* GS1-128 */
	test_testGenerateScanData(GS1_128_CCA, "^011231231231233310ABC123^99TESTING",
		"]C1011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(GS1_128_CCA,					// Composite uses ]e0
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

	gs1_encoder_free(ctx);

}


#define test_testProcessScanData(ss, sd, s, d) do {				\
	do_test_testProcessScanData(ctx, ss, sd, #s, gs1_encoder_s##s, d);	\
} while (0)

static void do_test_testProcessScanData(gs1_encoder *ctx, const bool should_succeed, const char *scanData,
		const char *expectSymName, const enum gs1_encoder_symbologies expectSym, const char *expectDataStr) {

	char casename[256];

	sprintf(casename, "%s", scanData);
	TEST_CASE(casename);

	TEST_CHECK(gs1_processScanData(ctx, scanData) ^ !should_succeed);
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

	// Digital Link URI
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

	// Digital Link URI
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

	gs1_encoder_free(ctx);

}


#endif  /* UNIT_TESTS */
