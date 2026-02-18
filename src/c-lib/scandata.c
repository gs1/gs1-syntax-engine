/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2021-2026 GS1 AISBL.
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
#include <string.h>

#include "syntax/gs1syntaxdictionary.h"
#include "enc-private.h"
#include "gs1encoders.h"
#include "dl.h"
#include "tr.h"


typedef enum {
	aiMode_AI,
	aiMode_NON_AI
} aiMode_t;


struct symIdEntry {
	char symId[3];
	aiMode_t aiMode;
	gs1_encoder_symbologies_t sym;
};


#define SYM(i, a, s) {		\
	.symId = i,		\
	.aiMode = aiMode_##a,	\
	.sym = s,		\
}

/*
 *  First matching instance in a lookup is the default
 *
 */
static const struct symIdEntry symIdTable[] = {
	SYM( "C1", AI,     gs1_encoder_sGS1_128_CCA        ),
	SYM( "C1", AI,     gs1_encoder_sGS1_128_CCC        ),
	SYM( "E0", NON_AI, gs1_encoder_sEAN13              ),
	SYM( "E0", AI,     gs1_encoder_sEAN13              ),
	SYM( "E0", NON_AI, gs1_encoder_sUPCA               ),
	SYM( "E0", AI,     gs1_encoder_sUPCA               ),
	SYM( "E0", NON_AI, gs1_encoder_sUPCE               ),
	SYM( "E0", AI,     gs1_encoder_sUPCE               ),
	SYM( "E4", NON_AI, gs1_encoder_sEAN8               ),
	SYM( "E4", AI,     gs1_encoder_sEAN8               ),
	SYM( "e0", AI,     gs1_encoder_sDataBarExpanded    ),
	SYM( "e0", AI,     gs1_encoder_sDataBarOmni        ),
	SYM( "e0", NON_AI, gs1_encoder_sDataBarOmni        ),
	SYM( "e0", AI,     gs1_encoder_sDataBarTruncated   ),
	SYM( "e0", NON_AI, gs1_encoder_sDataBarTruncated   ),
	SYM( "e0", AI,     gs1_encoder_sDataBarStacked     ),
	SYM( "e0", NON_AI, gs1_encoder_sDataBarStacked     ),
	SYM( "e0", AI,     gs1_encoder_sDataBarStackedOmni ),
	SYM( "e0", NON_AI, gs1_encoder_sDataBarStackedOmni ),
	SYM( "e0", AI,     gs1_encoder_sDataBarLimited     ),
	SYM( "e0", NON_AI, gs1_encoder_sDataBarLimited     ),
	// e0 is also shared with GS1-128 with CC
	SYM( "d1", NON_AI, gs1_encoder_sDM                 ),
	SYM( "d2", AI,     gs1_encoder_sDM                 ),
	SYM( "Q1", NON_AI, gs1_encoder_sQR                 ),
	SYM( "Q3", AI,     gs1_encoder_sQR                 ),
	SYM( "J0", NON_AI, gs1_encoder_sDotCode            ),
	SYM( "J1", AI,     gs1_encoder_sDotCode            ),
};

#undef SYM

#define CC_SYM_ID "]e0"


static const __ATTR_PURE char* lookupSymId(const gs1_encoder* const ctx) {

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


static size_t scancat(char* const out, const char* const in, const size_t out_len) {

	const char *p = in;
	char *q = out + out_len;

	if (*p == '^') {					// GS1 mode

		// Skip the leading FNC1 since we are following a symbology identifier
		while (*++p)
			*q++ = (*p == '^') ? '\x1D' : *p;	// Convert encoded FNC1 to GS

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

	return (size_t)(q - out);

}


static bool validateParity(uint8_t *str, size_t len) {

	int w;
	int parity = 0;
	size_t i;

	assert(*str);
	assert(len == strlen((char *)str));

	for (i = 0, w = ( len % 2 == 0 ? 3 : 1 ); i < len - 1; i++, w = 4 - w)
		parity += w * (str[i] - '0');

	parity = (10 - parity%10) % 10;

	if (parity + '0' == str[len-1]) return true;

	str[len-1] = (uint8_t)(parity + '0');		// Recalculate
	return false;

}


static bool checkAndNormalisePrimaryData(gs1_encoder* const ctx, const char *dataStr,
					 char* const outStr, size_t *outStr_len, int length) {

	size_t dataStr_len = strlen(dataStr);
	char *primary_out;

	if (dataStr_len != (size_t)(ctx->addCheckDigit ? length-1 : length)) {
		if (ctx->addCheckDigit)
			SET_ERR_V(PRIMARY_DATA_MUST_BE_N_DIGITS_WITHOUT_CHECK_DIGIT, length - 1);
		else
			SET_ERR_V(PRIMARY_DATA_MUST_BE_N_DIGITS, length);
		return false;
	}

	if (!gs1_allDigits((uint8_t*)dataStr, dataStr_len)) {
		SET_ERR(PRIMARY_DATA_MUST_BE_ALL_DIGITS);
		return false;
	}

	primary_out = outStr + *outStr_len;
	memcpy(primary_out, dataStr, dataStr_len);
	*outStr_len += dataStr_len;

	if (ctx->addCheckDigit)
		outStr[(*outStr_len)++] = '-';

	outStr[*outStr_len] = '\0';

	if (!validateParity((uint8_t*)primary_out, (size_t)(outStr + *outStr_len - primary_out)) &&
	    !ctx->addCheckDigit) {
		SET_ERR(PRIMARY_DATA_CHECK_DIGIT_IS_INCORRECT);
		return false;
	}

	return true;

}


char* gs1_generateScanData(gs1_encoder* const ctx) {

	char* cc = NULL;
	const char *pad;
	const char *dataStr;
	int length, aizeros;
	char* ret;
	size_t outStr_len = 0;
	const char* primary_data_out;

	assert(ctx);

	*ctx->outStr = '\0';

	if ((cc = strchr(ctx->dataStr, '|')) != NULL)		// Delimit end of linear data
		*cc++ = '\0';

	switch (ctx->sym) {

	case gs1_encoder_sQR:
	case gs1_encoder_sDM:
	case gs1_encoder_sDotCode:

		// QR:      "]Q1" for plain data; "]Q3" for GS1 data
		// DM:      "]d1" for plain data; "]d2" for GS1 data
		// DotCode: "]J0" for plain data; "]J1" for GS1 data

		// If plain data then put original faux CC delimiter back
		if (*ctx->dataStr != '^' && cc) {
			*--cc = '|';
			cc = NULL;
		}

		ctx->outStr[outStr_len++] = ']';
		memcpy(ctx->outStr + outStr_len, lookupSymId(ctx), 2);
		outStr_len += 2;
		ctx->outStr[outStr_len] = '\0';
		scancat(ctx->outStr, ctx->dataStr, outStr_len);
		break;

	case gs1_encoder_sGS1_128_CCA:
	case gs1_encoder_sGS1_128_CCC:

		if (!cc) {
			// "]C1" for linear-only GS1-128
			if (*ctx->dataStr != '^')
				goto fail;
			ctx->outStr[outStr_len++] = ']';
			memcpy(ctx->outStr + outStr_len, lookupSymId(ctx), 2);
			outStr_len += 2;
			ctx->outStr[outStr_len] = '\0';
			scancat(ctx->outStr, ctx->dataStr, outStr_len);
			break;
		}

		/* FALLTHROUGH */

	case gs1_encoder_sDataBarExpanded:  // And GS1-128 Composite

		// "]e0" followed by concatenated AI data from linear and CC
		if (*ctx->dataStr != '^')
			goto fail;
		memcpy(ctx->outStr + outStr_len, CC_SYM_ID, sizeof(CC_SYM_ID) - 1);
		outStr_len += sizeof(CC_SYM_ID) - 1;
		ctx->outStr[outStr_len] = '\0';
		outStr_len = scancat(ctx->outStr, ctx->dataStr, outStr_len);

		if (cc) {

			bool lastAIfnc1 = false;
			int i;

			if (*cc != '^')
				goto fail;

			// Append GS if last AI of linear component isn't fixed-length
			for (i = 0; i < ctx->numAIs && ctx->aiData[i].aiEntry; i++)
				lastAIfnc1 = ctx->aiData[i].aiEntry->fnc1;
			if (lastAIfnc1) {
				ctx->outStr[outStr_len++] = '\x1D';
				ctx->outStr[outStr_len] = '\0';
			}

			scancat(ctx->outStr, cc, outStr_len);

		}

		break;

	case gs1_encoder_sDataBarOmni:
	case gs1_encoder_sDataBarTruncated:
	case gs1_encoder_sDataBarStacked:
	case gs1_encoder_sDataBarStackedOmni:
	case gs1_encoder_sDataBarLimited:

		// "]e0" followed by concatenated AI data from linear and CC

		dataStr = ctx->dataStr;
		if (strncmp(dataStr, "^01", 3) == 0)
			dataStr += 3;

		ctx->outStr[outStr_len++] = ']';
		memcpy(ctx->outStr + outStr_len, lookupSymId(ctx), 2);
		outStr_len += 2;
		memcpy(ctx->outStr + outStr_len, "01", 2);
		outStr_len += 2;
		ctx->outStr[outStr_len] = '\0';

		primary_data_out = ctx->outStr + outStr_len;

		if (!checkAndNormalisePrimaryData(ctx, dataStr, ctx->outStr, &outStr_len, 14))
			goto fail;

		// GS1 DataBar Limited is restricted to low-valued inputs
		if (ctx->sym == gs1_encoder_sDataBarLimited) {
			if (primary_data_out[0] >= '2') {	// 14-digits must be less than 2 * 10^13
				SET_ERR(PRIMARY_DATA_IS_TOO_LARGE);
				goto fail;
			}
		}

		if (cc) {
			if (*cc != '^')
				goto fail;
			scancat(ctx->outStr, cc, outStr_len);
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
		if (strncmp(dataStr, "^01000000", (size_t)aizeros) == 0)
			dataStr += aizeros;

		ctx->outStr[outStr_len++] = ']';
		memcpy(ctx->outStr + outStr_len, lookupSymId(ctx), 2);
		outStr_len += 2;
		if (*pad)
			ctx->outStr[outStr_len++] = *pad;
		ctx->outStr[outStr_len] = '\0';

		if (!checkAndNormalisePrimaryData(ctx, dataStr, ctx->outStr, &outStr_len, length))
			goto fail;

		if (cc) {
			if (*cc != '^')
				goto fail;
			ctx->outStr[outStr_len++] = '|';
			memcpy(ctx->outStr + outStr_len, CC_SYM_ID, sizeof(CC_SYM_ID) - 1);
			outStr_len += sizeof(CC_SYM_ID) - 1;
			ctx->outStr[outStr_len] = '\0';		// "|" means start of new message
			scancat(ctx->outStr, cc, outStr_len);
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
	ctx->numSortedAIs = 0;

	ctx->err = gs1_encoder_eNO_ERROR;
	*ctx->errMsg = '\0';
	ctx->linterErr = GS1_LINTER_OK;
	*ctx->linterErrMarkup = '\0';

	if (*scanData != ']' || strlen(scanData) < 3) {
		SET_ERR(MISSING_SYMBOLOGY_IDENTIFIER);
		goto fail;
	}

	lookupSymAndModeBySymId(scanData + 1, &sym, &aiMode);

	if (sym == gs1_encoder_sNONE) {
		SET_ERR(UNSUPPORTED_SYMBOLOGY_IDENTIFIER);
		goto fail;
	}

	scanData += 3;

	if (strnlen(scanData, MAX_DATA) >= MAX_DATA) {
		SET_ERR_V(DATA_TOO_LONG, MAX_DATA - 1);
		goto fail;
	}

	ctx->sym = sym;
	p = ctx->dataStr;

	if (sym == gs1_encoder_sEAN13 || sym == gs1_encoder_sEAN8) {

		size_t primaryLen = (sym == gs1_encoder_sEAN13) ? 13 : 8;
		const char *cc = NULL;
		size_t scanData_len = strlen(scanData);

		if (scanData_len < primaryLen) {
			SET_ERR(PRIMARY_SCAN_DATA_IS_TOO_SHORT);
			goto fail;
		}

		if (scanData_len >= primaryLen + sizeof(CC_SYM_ID) &&
		    strncmp(scanData + primaryLen, "|" CC_SYM_ID, sizeof(CC_SYM_ID)) == 0) {
			cc = scanData + primaryLen + sizeof(CC_SYM_ID);
		} else if (scanData_len > primaryLen) {
			SET_ERR(PRIMARY_MESSAGE_IS_TOO_LONG);
			goto fail;
		}

		memcpy(p, scanData, primaryLen);
		p[primaryLen] = '\0';

		if (!gs1_allDigits((uint8_t*)p, primaryLen)) {
			SET_ERR(PRIMARY_MESSAGE_MAY_ONLY_CONTAIN_DIGITS);
			goto fail;
		}

		if (!validateParity((uint8_t*)p, primaryLen)) {
			SET_ERR(PRIMARY_MESSAGE_CHECK_DIGIT_IS_INCORRECT);
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
			SET_ERR(SCAN_DATA_CONTAINS_ILLEGAL_CARAT);
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
	if (strncmp(ctx->dataStr, "https://", 8) == 0 ||
	    strncmp(ctx->dataStr, "HTTPS://", 8) == 0 ||
	    strncmp(ctx->dataStr, "http://",  7) == 0 ||
	    strncmp(ctx->dataStr, "HTTP://",  7) == 0) {
		// We extract AIs with the element string stored in dlAIbuffer
		if (!gs1_parseDLuri(ctx, ctx->dataStr, ctx->dlAIbuffer))
			goto fail;
	}

	return true;

fail:

	*ctx->dataStr = '\0';
	ctx->sym = gs1_encoder_sNONE;
	if (*ctx->errMsg == '\0')
		SET_ERR(FAILED_TO_PROCESS_SCAN_DATA);

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

	TEST_CHECK(validateParity((uint8_t*)good_gtin14, strlen(good_gtin14)));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin14, strlen(bad_gtin14)));
	TEST_CHECK(bad_gtin14[13] == '5');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin13, strlen(good_gtin13)));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin13, strlen(bad_gtin13)));
	TEST_CHECK(bad_gtin13[12] == '7');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin12, strlen(good_gtin12)));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin12, strlen(bad_gtin12)));
	TEST_CHECK(bad_gtin12[11] == '8');		// Recomputed

	TEST_CHECK(validateParity((uint8_t*)good_gtin8, strlen(good_gtin8)));
	TEST_CHECK(!validateParity((uint8_t*)bad_gtin8, strlen(bad_gtin8)));
	TEST_CHECK(bad_gtin8[7] == '0');		// Recomputed

}


static void do_test_testGenerateScanData(gs1_encoder* const ctx, const char* const file, const int line, const char* const name, const gs1_encoder_symbologies_t sym, const char* const dataStr, const char* const expect) {

	char *out;
	char casename[256];

	snprintf(casename, sizeof(casename), "%s:%d: %s: %s", file, line, name, dataStr);
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

#define test_testGenerateScanData(n, d, e) do {							\
	do_test_testGenerateScanData(ctx, __FILE__, __LINE__, #n, gs1_encoder_s##n, d, e);	\
} while (0)

	test_testGenerateScanData(NONE, "", NULL);
	test_testGenerateScanData(NONE, "TESTING", NULL);

	/* QR */
	test_testGenerateScanData(QR, "TESTING", "]Q1TESTING");
	test_testGenerateScanData(QR, "\\^TESTING", "]Q1^TESTING");		// Escaped data "^" character
	test_testGenerateScanData(QR, "\\\\^TESTING", "]Q1\\^TESTING");		// Escaped data "\^" characters
	test_testGenerateScanData(QR, "^011231231231233310ABC123^99TESTING",
		"]Q3011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(QR, "^011231231231233310ABC123^99TESTING^",
		"]Q3011231231231233310ABC123" "\x1D" "99TESTING" "\x1D");	// Trailing FNC1 is not stripped

	/* DM */
	test_testGenerateScanData(DM, "TESTING", "]d1TESTING");
	test_testGenerateScanData(DM, "\\^TESTING", "]d1^TESTING");		// Escaped data "^" character
	test_testGenerateScanData(DM, "\\\\^TESTING", "]d1\\^TESTING");		// Escaped data "\^" characters
	test_testGenerateScanData(DM, "^011231231231233310ABC123^99TESTING",
		"]d2011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(DM, "^011231231231233310ABC123^99TESTING^",
		"]d2011231231231233310ABC123" "\x1D" "99TESTING" "\x1D");	// Trailing FNC1 is not stripped

	/* DotCode */
	test_testGenerateScanData(DotCode, "TESTING", "]J0TESTING");
	test_testGenerateScanData(DotCode, "\\^TESTING", "]J0^TESTING");	// Escaped data "^" character
	test_testGenerateScanData(DotCode, "\\\\^TESTING", "]J0\\^TESTING");	// Escaped data "\^" characters
	test_testGenerateScanData(DotCode, "^011231231231233310ABC123^99TESTING",
		"]J1011231231231233310ABC123" "\x1D" "99TESTING");
	test_testGenerateScanData(DotCode, "^011231231231233310ABC123^99TESTING^",
		"]J1011231231231233310ABC123" "\x1D" "99TESTING" "\x1D");	// Trailing FNC1 is not stripped

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


static void do_test_testProcessScanData(gs1_encoder* const ctx, const char* const file, const int line, const bool should_succeed, const char* const scanData,
		const char* const expectSymName, const gs1_encoder_symbologies_t expectSym, const char* const expectDataStr) {

	char casename[256];

	snprintf(casename, sizeof(casename), "%s:%d: %s", file, line, scanData);
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

#define test_testProcessScanData(ss, sd, s, d) do {							\
	do_test_testProcessScanData(ctx, __FILE__, __LINE__, ss, sd, #s, gs1_encoder_s##s, d);		\
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
	*ctx->dlAIbuffer = '\0';
	test_testProcessScanData(true, "]Q1https://example.com/01/12312312312333?99=TEST",
		QR, "https://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "^011231231231233399TEST") == 0);	// Check AI extraction

	/* QR Code with GS1 Digital Link URI (uppercase scheme) */
	*ctx->dlAIbuffer = '\0';
	test_testProcessScanData(true, "]Q1HTTPS://example.com/01/12312312312333?99=TEST",
		QR, "HTTPS://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "^011231231231233399TEST") == 0);	// Check AI extraction

	/* QR Code with GS1 Digital Link URI (forbidden mixed-case scheme) */
	*ctx->dlAIbuffer = '\0';
	test_testProcessScanData(true, "]Q1HtTps://example.com/01/12312312312333?99=TEST",
		QR, "HtTps://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "") == 0);	// Check AI extraction

	/* DM */
	test_testProcessScanData(true, "]d1", DM, "");
	test_testProcessScanData(true, "]d1TESTING", DM, "TESTING");
	test_testProcessScanData(true, "]d1^TESTING", DM, "\\^TESTING");
	test_testProcessScanData(true, "]d1\\^TESTING", DM, "\\\\^TESTING");
	test_testProcessScanData(false, "]d2", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]d2011231231231233310ABC123" "\x1D" "99TESTING",
		DM, "^011231231231233310ABC123^99TESTING");

	/* DM with GS1 Digital Link URI */
	*ctx->dlAIbuffer = '\0';
	test_testProcessScanData(true, "]d1https://example.com/01/12312312312333?99=TEST",
		DM, "https://example.com/01/12312312312333?99=TEST");
	TEST_CHECK(strcmp(ctx->dlAIbuffer, "^011231231231233399TEST") == 0);	// Check AI extraction

	/* DotCode */
	test_testProcessScanData(true, "]J0", DotCode, "");
	test_testProcessScanData(true, "]J0TESTING", DotCode, "TESTING");
	test_testProcessScanData(true, "]J0^TESTING", DotCode, "\\^TESTING");
	test_testProcessScanData(true, "]J0\\^TESTING", DotCode, "\\\\^TESTING");
	test_testProcessScanData(false, "]J1", NONE, "");		// Empty GS1 data
	test_testProcessScanData(true, "]J1011231231231233310ABC123" "\x1D" "99TESTING",
		DotCode, "^011231231231233310ABC123^99TESTING");

	/* DotCode with GS1 Digital Link URI - carrier is suitable, but no active applications support this */
	*ctx->dlAIbuffer = '\0';
	test_testProcessScanData(true, "]J0https://example.com/01/12312312312333?99=TEST",
		DotCode, "https://example.com/01/12312312312333?99=TEST");
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
