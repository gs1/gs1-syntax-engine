/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2021-2025 GS1 AISBL.
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
#include "scandata.h"
#include "syn.h"
#include "tr.h"


static inline void reset_error(gs1_encoder* const ctx) {
	assert(ctx);
	ctx->err = gs1_encoder_eNO_ERROR;
	*ctx->errMsg = '\0';
	ctx->linterErr = GS1_LINTER_OK;
	*ctx->linterErrMarkup = '\0';
}


__ATTR_CONST size_t gs1_encoder_instanceSize(void) {
	return sizeof(struct gs1_encoder);
}


__ATTR_CONST int gs1_encoder_getMaxDataStrLength(void) {
	return MAX_DATA;
}


gs1_encoder* gs1_encoder_init(void* const mem) {

	gs1_encoder *ctx = NULL;

	if (!mem) {  // No storage provided so allocate our own
#ifndef NOMALLOC
		ctx = GS1_ENCODERS_MALLOC(sizeof(gs1_encoder));
#endif
		if (ctx == NULL) return NULL;
	} else {  // Use the provided storage
		ctx = mem;
	}

	// Set default parameters
	ctx = memcpy(ctx, (&(struct gs1_encoder) {
		.localAlloc = !mem,
		.sym = gs1_encoder_sNONE,
		.addCheckDigit = false,
		.permitUnknownAIs = false,
		.permitZeroSuppressedGTINinDLuris = false,
		.includeDataTitlesInHRI = false,
		.aiTable = NULL,
		.aiTableEntries = 0,
		.aiTableIsDynamic = false,
		.dlKeyQualifiers = NULL,
		.numDLkeyQualifiers = 0,
		.numAIs = 0,
		.dataStr = { 0 },
		.errMsg = { 0 },
		.linterErr = GS1_LINTER_OK,
		.linterErrMarkup = { 0 }
	}), sizeof(struct gs1_encoder));

	gs1_loadSyntaxDictionary(ctx, NULL);
	gs1_loadValidationTable(ctx);

	return ctx;

}


void gs1_encoder_free(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);

	if (ctx->aiTable && ctx->aiTableIsDynamic) {
		gs1_freeSyntaxDictionaryEntries(ctx, ctx->aiTable);
		GS1_ENCODERS_FREE(ctx->aiTable);
	}

	gs1_freeDLkeyQualifiers(ctx);
	if (ctx->localAlloc)
		GS1_ENCODERS_FREE(ctx);
}


__ATTR_CONST char* gs1_encoder_getVersion(void) {
	return __DATE__;
}


gs1_encoder_symbologies_t gs1_encoder_getSym(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->sym;
}
bool gs1_encoder_setSym(gs1_encoder* const ctx, const gs1_encoder_symbologies_t sym) {
	assert(ctx);
	reset_error(ctx);
	if (sym < gs1_encoder_sNONE || sym >= gs1_encoder_sNUMSYMS) {
		SET_ERR(UNKNOWN_SYMBOLOGY);
		return false;
	}
	ctx->sym = sym;
	return true;
}


bool gs1_encoder_getAddCheckDigit(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->addCheckDigit;
}
bool gs1_encoder_setAddCheckDigit(gs1_encoder* const ctx, const bool addCheckDigit) {
	assert(ctx);
	reset_error(ctx);
	ctx->addCheckDigit = addCheckDigit;
	return true;
}


bool gs1_encoder_getPermitZeroSuppressedGTINinDLuris(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->permitZeroSuppressedGTINinDLuris;
}
bool gs1_encoder_setPermitZeroSuppressedGTINinDLuris(gs1_encoder* const ctx, const bool permitZeroSuppressedGTINinDLuris) {
	assert(ctx);
	reset_error(ctx);
	ctx->permitZeroSuppressedGTINinDLuris = permitZeroSuppressedGTINinDLuris;
	return true;
}


bool gs1_encoder_getPermitUnknownAIs(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->permitUnknownAIs;
}
bool gs1_encoder_setPermitUnknownAIs(gs1_encoder* const ctx, const bool permitUnknownAIs) {
	assert(ctx);
	reset_error(ctx);
	ctx->permitUnknownAIs = permitUnknownAIs;
	return true;
}


bool gs1_encoder_getValidateAIassociations(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->validationTable[gs1_encoder_vREQUISITE_AIS].enabled;
}
bool gs1_encoder_setValidateAIassociations(gs1_encoder* const ctx, const bool validateAIassociations) {
	assert(ctx);
	reset_error(ctx);
	assert(!ctx->validationTable[gs1_encoder_vREQUISITE_AIS].locked);
	ctx->validationTable[gs1_encoder_vREQUISITE_AIS].enabled = validateAIassociations;
	return true;
}


bool gs1_encoder_getValidationEnabled(gs1_encoder* const ctx, const gs1_encoder_validations_t validation) {
	assert(ctx);
	reset_error(ctx);
	return ctx->validationTable[validation].enabled;
}
bool gs1_encoder_setValidationEnabled(gs1_encoder* const ctx, const gs1_encoder_validations_t validation, const bool enabled) {
	assert(ctx);
	reset_error(ctx);
	if ((signed int)validation < 0 || validation >= gs1_encoder_vNUMVALIDATIONS) {  // Cast satisfies "unsigned enum < 0" checks
		SET_ERR(UNKNOWN_VALIDATION);
		return false;
	}
	if (ctx->validationTable[validation].locked) {
		SET_ERR(VALIDATION_CANNOT_BE_AMENDED);
		return false;
	}
	ctx->validationTable[validation].enabled = enabled;
	return true;
}


bool gs1_encoder_getIncludeDataTitlesInHRI(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->includeDataTitlesInHRI;
}
bool gs1_encoder_setIncludeDataTitlesInHRI(gs1_encoder* const ctx, const bool includeDataTitles) {
	assert(ctx);
	reset_error(ctx);
	ctx->includeDataTitlesInHRI = includeDataTitles;
	return true;
}


char* gs1_encoder_getDataStr(gs1_encoder* const ctx) {
	assert(ctx);
	reset_error(ctx);
	return ctx->dataStr;
}
bool gs1_encoder_setDataStr(gs1_encoder* const ctx, const char* const dataStr) {

	char *cc;

	assert(ctx);
	assert(dataStr);
	reset_error(ctx);

	if (strlen(dataStr) > MAX_DATA) {
		SET_ERR_V(DATA_TOO_LONG, MAX_DATA);
		return false;
	}
	if (ctx->dataStr != dataStr)					// File input is via ctx->dataStr
		strcpy(ctx->dataStr, dataStr);

	// Validate and process data, including extraction of HRI
	ctx->numAIs = 0;
	if ((strlen(ctx->dataStr) >= 8 && strncmp(ctx->dataStr, "https://", 8) == 0) ||	// GS1 Digital Link URI
	    (strlen(ctx->dataStr) >= 8 && strncmp(ctx->dataStr, "HTTPS://", 8) == 0) ||
	    (strlen(ctx->dataStr) >= 7 && strncmp(ctx->dataStr, "http://",  7) == 0) ||
	    (strlen(ctx->dataStr) >= 7 && strncmp(ctx->dataStr, "HTTP://",  7) == 0)) {
		// We extract AIs with the element string stored in dlAIbuffer
		if (!gs1_parseDLuri(ctx, ctx->dataStr, ctx->dlAIbuffer))
			goto fail;
	}
	else if ((cc = strchr(ctx->dataStr, '|')) != NULL) {		// Composite symbol

		*cc = '\0';						// Delimit end of linear component

		if (*ctx->dataStr == '^' && !gs1_processAIdata(ctx, ctx->dataStr, true))
			goto fail;

		if (ctx->numAIs >= MAX_AIS) {
			SET_ERR(TOO_MANY_AIS);
			goto fail;
		}

		// Indicate separator in HRI
		ctx->aiData[ctx->numAIs].kind = aiValue_ccsep;
		ctx->numAIs++;

		if (!gs1_processAIdata(ctx, cc + 1, true))
			goto fail;

		*cc = '|';						// Restore orginal "|"

	}
	else {								// Linear-only symbol
		if (*ctx->dataStr == '^' && !gs1_processAIdata(ctx, ctx->dataStr, true))
			goto fail;
	}

	if (!gs1_validateAIs(ctx))
		goto fail;

	return true;

fail:

	*ctx->dataStr = '\0';
	ctx->numAIs = 0;
	return false;

}


bool gs1_encoder_setAIdataStr(gs1_encoder* const ctx, const char* const aiData) {

	char *cc;

	assert(ctx);
	assert(aiData);
	reset_error(ctx);

	// Validate AI data
	ctx->numAIs = 0;
	if ((cc = strchr(aiData, '|')) != NULL)		// Composite symbol
	{

		*cc = '\0';					// Delimit end of linear component

		if (!gs1_parseAIdata(ctx, aiData, ctx->dataStr))
			goto fail;

		if (ctx->numAIs >= MAX_AIS) {
			SET_ERR(TOO_MANY_AIS);
			goto fail;
		}

		strcat(ctx->dataStr, "|");

		// Indicate separator in HRI
		ctx->aiData[ctx->numAIs].kind = aiValue_ccsep;
		ctx->numAIs++;

		if (!gs1_parseAIdata(ctx, cc+1, ctx->dataStr + strlen(ctx->dataStr)))
			goto fail;

		*cc = '|';					// Restore orginal "|"

	}
	else {							// Linear-only symbol
		if (!gs1_parseAIdata(ctx, aiData, ctx->dataStr))
			goto fail;
	}

	if (!gs1_validateAIs(ctx))
		goto fail;

	return true;

fail:

	*ctx->dataStr = '\0';
	ctx->numAIs = 0;
	return false;

}


char* gs1_encoder_getAIdataStr(gs1_encoder* const ctx) {

	int i, j;
	char *p = ctx->outStr;

	assert(ctx);
	assert(ctx->numAIs <= MAX_AIS);
	reset_error(ctx);

	if (ctx->numAIs == 0)		// Not GS1 data
		return NULL;

	for (i = 0; i < ctx->numAIs; i++) {
		const struct aiValue *ai = &ctx->aiData[i];
		if (ai->kind == aiValue_aival) {
			int n = snprintf(p, sizeof(ctx->outStr) - (size_t)(p - ctx->outStr), "(%.*s)", ai->ailen, ai->ai);
			assert(n >= 0 || n < (int)(sizeof(ctx->outStr) - (size_t)(p - ctx->outStr)));
			p += n;
			for (j = 0; j < ai->vallen; j++) {
				if (ai->value[j] == '(')	// Escape data "("
					*p++ = '\\';
				*p++ = ai->value[j];
			}
		} else if (ai->kind == aiValue_ccsep) {
			*p++ = '|';
		}	// Otherwise ignored parameters
	}
	*p = '\0';

	return ctx->outStr;

}


char* gs1_encoder_getDLuri(gs1_encoder* const ctx, const char* const stem) {
	assert(ctx);
	return gs1_generateDLuri(ctx, stem);
}


char* gs1_encoder_getScanData(gs1_encoder* const ctx) {
	assert(ctx);
	return gs1_generateScanData(ctx);
}


bool gs1_encoder_setScanData(gs1_encoder* const ctx, const char* const scanData) {
	assert(ctx);
	assert(scanData);

	if (!gs1_processScanData(ctx, scanData))
		goto fail;

	if (!gs1_validateAIs(ctx))
		goto fail;

	return true;

fail:

	return false;

}


int gs1_encoder_getHRI(gs1_encoder* const ctx, char*** const out) {

	int i, j;
	char *p = ctx->outStr;

	assert(ctx);
	assert(ctx->numAIs <= MAX_AIS);
	reset_error(ctx);

	*p = '\0';
	for (i = 0, j = 0; i < ctx->numAIs; i++) {

		const struct aiValue* const ai = &ctx->aiData[i];
		int n;

		if (ai->kind != aiValue_aival)
			continue;

		assert(ai->aiEntry);

		ctx->outHRI[j] = p;

		if (!ctx->includeDataTitlesInHRI || *ai->aiEntry->title == '\0')
			n = snprintf(p, sizeof(ctx->outStr) - (size_t)(p - ctx->outStr), "(%.*s) %.*s", ai->ailen, ai->ai, ai->vallen, ai->value);
		else
			n = snprintf(p, sizeof(ctx->outStr) - (size_t)(p - ctx->outStr), "%s (%.*s) %.*s", ai->aiEntry->title, ai->ailen, ai->ai, ai->vallen, ai->value);
		assert(n >= 0 && n < (int)(sizeof(ctx->outStr) - (size_t)(p - ctx->outStr)));
		p += n;

		*p++ = '\0';

		j++;

	}

	*out = ctx->outHRI;

	return j;

}


size_t gs1_encoder_getHRIsize(gs1_encoder* const ctx) {

	size_t sz = 0;
	int i;
	char **hri;
	const int numhri = gs1_encoder_getHRI(ctx, &hri);

	for (i = 0; i < numhri; i++)
		sz += strlen(hri[i]) + 1;  // Includes "|" or NULL terminator

	return sz;

}


void gs1_encoder_copyHRI(gs1_encoder* const ctx, void* const buf, const size_t max) {

	char *p;
	char **hri;
	int i, numhri;
	int rem = (int)max;

	assert(ctx);
	reset_error(ctx);

	numhri = gs1_encoder_getHRI(ctx, &hri);

	p = buf;
	*p = '\0';
	for (i = 0; i < numhri; i++) {
		rem -= (int)strlen(hri[i]) + 1;
		if (rem < 0) {
			*p = '\0';
			return;
		}
		if (i != 0)
			strcat(p, "|");
		strcat(p, hri[i]);
	}

	return;

}


int gs1_encoder_getDLignoredQueryParams(gs1_encoder* const ctx, char*** const out) {

	int i, j;
	char *p = ctx->outStr;

	assert(ctx);
	assert(ctx->numAIs <= MAX_AIS);
	reset_error(ctx);

	*p = '\0';
	for (i = 0, j = 0; i < ctx->numAIs; i++) {

		const struct aiValue* const ai = &ctx->aiData[i];
		int n;

		if (ai->kind != alValue_dlign)
			continue;

		ctx->outHRI[j] = p;

		n = snprintf(p, sizeof(ctx->outStr) - (size_t)(p - ctx->outStr), "%.*s", ai->vallen, ai->value);
		assert(n >= 0 && n < (int)(sizeof(ctx->outStr) - (size_t)(p - ctx->outStr)));
		p += n;

		*p++ = '\0';

		j++;

	}

	*out = ctx->outHRI;
	return j;

}


size_t gs1_encoder_getDLignoredQueryParamsSize(gs1_encoder* const ctx) {

	size_t sz = 0;
	int i;
	char **qp;
	const int numqp = gs1_encoder_getDLignoredQueryParams(ctx, &qp);

	for (i = 0; i < numqp; i++)
		sz += strlen(qp[i]) + 1;  // Includes "&" or NULL terminator

	return sz;

}


void gs1_encoder_copyDLignoredQueryParams(gs1_encoder* const ctx, void* const buf, const size_t max) {

	char *p;
	char **qp;
	int i, numqp;
	int rem = (int)max;

	assert(ctx);
	reset_error(ctx);

	numqp = gs1_encoder_getDLignoredQueryParams(ctx, &qp);

	p = buf;
	*p = '\0';
	for (i = 0; i < numqp; i++) {
		rem -= (int)strlen(qp[i]) + 1;
		if (rem < 0) {
			*p = '\0';
			return;
		}
		if (i != 0)
			strcat(p, "&");
		strcat(p, qp[i]);
	}

	return;

}


__ATTR_PURE char* gs1_encoder_getErrMsg(gs1_encoder* const ctx) {
	assert(ctx);
	return ctx->errMsg;
}


__ATTR_PURE char* gs1_encoder_getErrMarkup(gs1_encoder* const ctx) {
	assert(ctx);
	return ctx->linterErrMarkup;
}


/*
 *  Utility functions
 *
 */

__ATTR_PURE bool gs1_allDigits(const uint8_t* const str, size_t len) {

	size_t i;

	assert(str);

	if (!len)
		len = strlen((char *)str);

	for (i = 0; i < len; i++) {
		if (str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;

}


char* gs1_strdup_alloc(const char *s) {

	size_t len = strlen(s) + 1;
	void *d = GS1_ENCODERS_MALLOC(len);

	if (!d)
		return NULL;

	return memcpy(d, s, len);

}


#ifdef UNIT_TESTS

#define TEST_NO_MAIN
#include "acutest.h"

// Used to test compile-time buffer allocation for the gs1encoder instance
static uint8_t static_buf[sizeof(gs1_encoder)];

// Sizable buffer on the heap so that we don't exhaust the stack
char bigbuffer[MAX_DATA+2];


void test_api_getVersion(void) {
	char *version = gs1_encoder_getVersion();

	TEST_CHECK(version != NULL && strcmp(version, __DATE__) == 0);
}


void test_api_instanceSize(void) {
	TEST_CHECK(gs1_encoder_instanceSize() == sizeof(struct gs1_encoder));
}


void test_api_init(void) {

	gs1_encoder* ctx;
	void *heap;
	size_t mem;

	// Mallocs its own memory
	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNONE);
	gs1_encoder_free(ctx);

	// We malloc the storage on the heap at run-time and pass it in
#ifndef __clang_analyzer__
// Analyzer fails to derive that ctx->localAlloc will be set and incorrectly reports a double free
	TEST_ASSERT((mem = gs1_encoder_instanceSize()) > 0);
	TEST_ASSERT((heap = GS1_ENCODERS_MALLOC(mem)) != NULL);
	TEST_ASSERT((ctx = gs1_encoder_init(heap)) == heap);
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNONE);
	gs1_encoder_free(ctx);
	GS1_ENCODERS_FREE(heap);
#endif

	// We allocate at compile-time and pass it in
	TEST_ASSERT((ctx = gs1_encoder_init(&static_buf)) != NULL);
	assert(ctx);
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNONE);

}


void test_api_defaults(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNONE);
	TEST_CHECK(strcmp(gs1_encoder_getDataStr(ctx), "") == 0);

	gs1_encoder_free(ctx);

}


void test_api_sym(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(!gs1_encoder_setSym(ctx, gs1_encoder_sNONE - 1));     // Too small

	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sNONE));          // First
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNONE);

	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarOmni));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarOmni);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarTruncated));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarTruncated);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarStacked));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarStacked);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarStackedOmni));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarStackedOmni);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarLimited));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarLimited);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarExpanded));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarExpanded);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sUPCA));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sUPCA);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sUPCE));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sUPCE);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sEAN13));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sEAN13);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sEAN8));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sEAN8);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sGS1_128_CCA));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sGS1_128_CCA);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sGS1_128_CCC));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sGS1_128_CCC);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sQR));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sQR);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDM));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDM);
	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sDotCode));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDotCode);

	TEST_CHECK(gs1_encoder_setSym(ctx, gs1_encoder_sNUMSYMS - 1));   // Last
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sNUMSYMS - 1);

	TEST_CHECK(!gs1_encoder_setSym(ctx, gs1_encoder_sNUMSYMS));      // Too big

	TEST_CHECK(gs1_encoder_sNUMSYMS == 15);  // Remember to change when adding new symbologies

	gs1_encoder_free(ctx);

}


void test_api_addCheckDigit(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(!gs1_encoder_getAddCheckDigit(ctx));		// Default

	TEST_CHECK(gs1_encoder_setAddCheckDigit(ctx, true));	// Set
	TEST_CHECK(gs1_encoder_getAddCheckDigit(ctx));

	TEST_CHECK(gs1_encoder_setAddCheckDigit(ctx, false));	// Reset
	TEST_CHECK(!gs1_encoder_getAddCheckDigit(ctx));

	gs1_encoder_free(ctx);

}


void test_api_permitUnknownAIs(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(!gs1_encoder_getPermitUnknownAIs(ctx));		// Default

	TEST_CHECK(gs1_encoder_setPermitUnknownAIs(ctx, true));		// Set
	TEST_CHECK(gs1_encoder_getPermitUnknownAIs(ctx));

	TEST_CHECK(gs1_encoder_setPermitUnknownAIs(ctx, false));	// Reset
	TEST_CHECK(!gs1_encoder_getPermitUnknownAIs(ctx));

	gs1_encoder_free(ctx);

}


void test_api_permitZeroSuppressedGTINinDLuris(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(!gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx));		// Default

	TEST_CHECK(gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, true));		// Set
	TEST_CHECK(gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx));

	TEST_CHECK(gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx, false));	// Reset
	TEST_CHECK(!gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx));

	gs1_encoder_free(ctx);

}


void test_api_validateAIassociations(void) {

DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	/*
	 *  Legacy function that now updates gs1_encoder_vREQUISITE_AIS, but
	 *  not gs1_encoder_vMUTEX_AIS.
	 *
	 */

	TEST_CHECK(gs1_encoder_getValidateAIassociations(ctx));		// Default
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vMUTEX_AIS));

	TEST_CHECK(gs1_encoder_setValidateAIassociations(ctx, false));	// Set
	TEST_CHECK(!gs1_encoder_getValidateAIassociations(ctx));
	TEST_CHECK(!gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vMUTEX_AIS));	// Untouched

	TEST_CHECK(gs1_encoder_setValidateAIassociations(ctx, true));	// Reset
	TEST_CHECK(gs1_encoder_getValidateAIassociations(ctx));
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vMUTEX_AIS));

	gs1_encoder_free(ctx);

DIAG_POP

}


void test_api_validations(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// Unlocked validation that is enabled by default
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));		// Default
	TEST_CHECK(gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, false));
	TEST_CHECK(!gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));
	TEST_CHECK(gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS, true));	// Reset
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREQUISITE_AIS));

	// Locked validation (always enabled)
	TEST_CHECK(gs1_encoder_getValidationEnabled(ctx, gs1_encoder_vREPEATED_AIS));		// Default
	TEST_CHECK(!gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vREPEATED_AIS, false));

	gs1_encoder_free(ctx);

}


void test_api_dataStr(void) {

	gs1_encoder* ctx;
	int i;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_CHECK(gs1_encoder_setDataStr(ctx, "barcode"));
	TEST_CHECK(strcmp(gs1_encoder_getDataStr(ctx), "barcode") == 0);
	TEST_CHECK(gs1_encoder_setDataStr(ctx, ""));
	TEST_CHECK(gs1_encoder_setDataStr(ctx, "a"));
	TEST_CHECK(gs1_encoder_setDataStr(ctx, "41512312312312328020REF00112991225|^99123123"));
	TEST_CHECK(gs1_encoder_setDataStr(ctx, "^41512312312312328020REF00112991225|^99123123"));

	for (i = 0; i <= MAX_DATA; i++) {
		bigbuffer[i]='a';
	}
	bigbuffer[MAX_DATA+1]='\0';
	TEST_CHECK(!gs1_encoder_setDataStr(ctx, bigbuffer));  // Too long

	bigbuffer[MAX_DATA]='\0';
	TEST_CHECK(gs1_encoder_setDataStr(ctx, bigbuffer));   // Maximun length

	gs1_encoder_free(ctx);

}


void test_api_getAIdataStr(void) {

	gs1_encoder* ctx;
	char *out;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// Plain data
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "TESTING"));
	TEST_CHECK((out = gs1_encoder_getAIdataStr(ctx)) == NULL);	// No AI extraction
	TEST_CHECK((out = gs1_encoder_getDataStr(ctx)) != NULL);	// Just hold the data string
	assert(out);
	TEST_CHECK(strcmp(out, "TESTING") == 0);

	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123"));
	TEST_ASSERT((out = gs1_encoder_getAIdataStr(ctx)) != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, "(01)12312312312333(10)ABC123") == 0);

	// Escape data "(" characters
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC(123"));
	TEST_ASSERT((out = gs1_encoder_getAIdataStr(ctx)) != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, "(01)12312312312333(10)ABC\\(123") == 0);

	// Composite strings
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123|^99XYZ(TM)_CORP"));
	TEST_ASSERT((out = gs1_encoder_getAIdataStr(ctx)) != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, "(01)12312312312333(10)ABC123|(99)XYZ\\(TM)_CORP") == 0);

	// GS1 DL URI
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://example.org/01/12312312312333/10/ABC123"));
	TEST_ASSERT((out = gs1_encoder_getAIdataStr(ctx)) != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, "(01)12312312312333(10)ABC123") == 0);

	// Mixed-case scheme is not a valid DL URI, so plain data
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "HtTp://example.org/01/12312312312333/10/ABC123"));
	TEST_ASSERT((out = gs1_encoder_getAIdataStr(ctx)) == NULL);	// No AI extraction
	TEST_CHECK((out = gs1_encoder_getDataStr(ctx)) != NULL);	// Just a plain data string
	assert(out);
	TEST_CHECK(strcmp(out, "HtTp://example.org/01/12312312312333/10/ABC123") == 0);

	gs1_encoder_free(ctx);

}


void test_api_getScanData(void) {

	gs1_encoder* ctx;
	char *out;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_ASSERT(gs1_encoder_setSym(ctx, gs1_encoder_sDataBarExpanded));
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123^11991225|^98COMPOSITE^97XYZ"));
	TEST_ASSERT((out = gs1_encoder_getScanData(ctx)) != NULL);
	assert(out);
	TEST_CHECK(strcmp(out, "]e0011231231231233310ABC123" "\x1D" "1199122598COMPOSITE" "\x1D" "97XYZ") == 0);

	gs1_encoder_free(ctx);

}


void test_api_setScanData(void) {

	gs1_encoder* ctx;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	TEST_ASSERT(gs1_encoder_setScanData(ctx, "]e0011231231231233310ABC123" "\x1D" "99XYZ"));
	TEST_CHECK(gs1_encoder_getSym(ctx) == gs1_encoder_sDataBarExpanded);
	TEST_CHECK(strcmp(gs1_encoder_getDataStr(ctx), "^011231231231233310ABC123^99XYZ") == 0);

	gs1_encoder_free(ctx);

}


void test_api_getHRI(void) {

	gs1_encoder* ctx;
	int numAIs;
	char **hri;
	char buf[256];

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// HRI from linear-only, raw AI data
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 2);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(10) ABC123") == 0);

	// HRI from composite, raw AI data
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123|^99COMPOSITE"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 3);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(10) ABC123") == 0);
	TEST_CHECK(strcmp(hri[2], "(99) COMPOSITE") == 0);

	// HRI from linear-only, bracketed AI data
	strcpy(buf, "(01)12312312312333(10)ABC123");
	TEST_ASSERT(gs1_encoder_setAIdataStr(ctx, buf));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 2);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(10) ABC123") == 0);

	// HRI from composite, bracketed AI data
	strcpy(buf, "(01)12312312312333(10)ABC123|(99)COMPOSITE");
	TEST_ASSERT(gs1_encoder_setAIdataStr(ctx, buf));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 3);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(10) ABC123") == 0);
	TEST_CHECK(strcmp(hri[2], "(99) COMPOSITE") == 0);

	// HRI from GS1 Digital Link URI
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?99=ABC%2d123&98=XYZ"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 4);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(22) TESTING") == 0);
	TEST_CHECK(strcmp(hri[2], "(99) ABC-123") == 0);
	TEST_CHECK(strcmp(hri[3], "(98) XYZ") == 0);

	// HRI from GS1 Digital Link URI with extra ignored query params
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?singleton&99=ABC%2d123&compound1=QWERTY&98=XYZ&compound2=12345"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 4);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(22) TESTING") == 0);
	TEST_CHECK(strcmp(hri[2], "(99) ABC-123") == 0);
	TEST_CHECK(strcmp(hri[3], "(98) XYZ") == 0);

	// HRI from data with unknown AIs: (88), (89)
	gs1_encoder_setPermitUnknownAIs(ctx, true);

	strcpy(buf, "(01)12312312312333(89)ABC123|(88)COMPOSITE");
	TEST_ASSERT(gs1_encoder_setAIdataStr(ctx, buf));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 3);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(89) ABC123") == 0);
	TEST_CHECK(strcmp(hri[2], "(88) COMPOSITE") == 0);

	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR, false);
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333?89=TESTING&99=ABC%2d123&88=XYZ"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 4);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "(89) TESTING") == 0);
	TEST_CHECK(strcmp(hri[2], "(99) ABC-123") == 0);
	TEST_CHECK(strcmp(hri[3], "(88) XYZ") == 0);
	gs1_encoder_setValidationEnabled(ctx, gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR, true);

	// HRI from linear-only, raw AI data, with unknown AI with known length 3
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^236ABC123"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 1);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(236) ABC123") == 0);

	// HRI from linear-only, raw AI data, with unknown AI with known length 4
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^8299ABC123"));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 1);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "(8299) ABC123") == 0);

	gs1_encoder_setIncludeDataTitlesInHRI(ctx, true);

	// HRI from composite, bracketed AI data, with data titles
	strcpy(buf, "(01)12312312312333(10)ABC123|(99)COMPOSITE");
	TEST_ASSERT(gs1_encoder_setAIdataStr(ctx, buf));
	TEST_ASSERT((numAIs = gs1_encoder_getHRI(ctx, &hri)) == 3);
	TEST_ASSERT(hri != NULL);
	assert(hri);
	TEST_CHECK(strcmp(hri[0], "GTIN (01) 12312312312333") == 0);
	TEST_CHECK(strcmp(hri[1], "BATCH/LOT (10) ABC123") == 0);
	TEST_CHECK(strcmp(hri[2], "INTERNAL (99) COMPOSITE") == 0);

	gs1_encoder_free(ctx);

}


void test_api_copyHRI(void) {

DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS

	gs1_encoder* ctx;
	char buf[64];
	size_t needed;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// No HRI should return empty string
	gs1_encoder_copyHRI(ctx, (void*)buf, sizeof(buf));
	TEST_CHECK(buf[0] == '\0');

	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?99=ABC%2d123&98=XYZ"));
	TEST_CHECK((needed = gs1_encoder_getHRIsize(ctx)) == 55);
	gs1_encoder_copyHRI(ctx, (void*)buf, sizeof(buf));
	TEST_CHECK(strcmp(buf, "(01) 12312312312333|(22) TESTING|(99) ABC-123|(98) XYZ") == 0);
	TEST_CHECK(strlen(buf) == needed - 1);

	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "^011231231231233310ABC123|^99COMPOSITE"));
	TEST_CHECK((needed = gs1_encoder_getHRIsize(ctx)) == 47);
	gs1_encoder_copyHRI(ctx, (void*)buf, sizeof(buf));
	TEST_CHECK(strcmp(buf, "(01) 12312312312333|(10) ABC123|(99) COMPOSITE") == 0);
	TEST_CHECK(strlen(buf) == needed - 1);

	// Check buffer too short returns empty string
	gs1_encoder_copyHRI(ctx, (void*)buf, needed - 1);
	TEST_CHECK(buf[0] == '\0');

	gs1_encoder_free(ctx);

DIAG_POP

}


void test_api_getDLignoredQueryParams(void) {

	gs1_encoder* ctx;
	int numAIs;
	char **qp;
	char buf[256];

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// No ignored query params from non-DL URI data
	strcpy(buf, "(01)12312312312333(10)ABC123|(99)COMPOSITE");
	TEST_ASSERT(gs1_encoder_setAIdataStr(ctx, buf));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 0);
	TEST_ASSERT(qp != NULL);

	// No extra query parameters given
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?99=ABC%2d123&98=XYZ"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 0);
	TEST_ASSERT(qp != NULL);

	// Non-numeric singleton before
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?singleton&99=ABC%2d123"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 1);
	TEST_ASSERT(qp != NULL);
	assert(qp);
	TEST_CHECK(strcmp(qp[0], "singleton") == 0);

	// Non-numeric singleton after
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?99=ABC%2d123&singleton"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 1);
	TEST_ASSERT(qp != NULL);
	assert(qp);
	TEST_CHECK(strcmp(qp[0], "singleton") == 0);

	// Non-numeric compound before
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?compound1=QWERTY&99=ABC%2d123"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 1);
	TEST_ASSERT(qp != NULL);
	assert(qp);
	TEST_CHECK(strcmp(qp[0], "compound1=QWERTY") == 0);

	// Non-numeric compound after
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?99=ABC%2d123&compound1=QWERTY"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 1);
	TEST_ASSERT(qp != NULL);
	assert(qp);
	TEST_CHECK(strcmp(qp[0], "compound1=QWERTY") == 0);

	// Singleton and compond non-numeric query parameters before and after
	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?singleton1&compound1=QWERTY&99=ABC%2d123&singleton2&98=XYZ&compound2=12345"));
	TEST_ASSERT((numAIs = gs1_encoder_getDLignoredQueryParams(ctx, &qp)) == 4);
	TEST_ASSERT(qp != NULL);
	assert(qp);
	TEST_CHECK(strcmp(qp[0], "singleton1") == 0);
	TEST_CHECK(strcmp(qp[1], "compound1=QWERTY") == 0);
	TEST_CHECK(strcmp(qp[2], "singleton2") == 0);
	TEST_CHECK(strcmp(qp[3], "compound2=12345") == 0);

	gs1_encoder_free(ctx);

}


void test_api_copyDLignoredQueryParams(void) {

DIAG_PUSH
DIAG_DISABLE_DEPRECATED_DECLARATIONS

	gs1_encoder* ctx;
	char buf[64];
	size_t needed;

	TEST_ASSERT((ctx = gs1_encoder_init(NULL)) != NULL);
	assert(ctx);

	// No data should return empty string
	gs1_encoder_copyDLignoredQueryParams(ctx, (void*)buf, sizeof(buf));
	TEST_CHECK(buf[0] == '\0');

	TEST_ASSERT(gs1_encoder_setDataStr(ctx, "https://a/01/12312312312333/22/TESTING?singleton1&compound1=QWERTY&99=ABC%2d123&singleton2&98=XYZ&compound2=12345"));
	TEST_CHECK((needed = gs1_encoder_getDLignoredQueryParamsSize(ctx)) == 55);
	gs1_encoder_copyDLignoredQueryParams(ctx, (void*)buf, sizeof(buf));
	TEST_CHECK(strcmp(buf, "singleton1&compound1=QWERTY&singleton2&compound2=12345") == 0);
	TEST_CHECK(strlen(buf) == needed - 1);

	// Check buffer too short returns empty string
	gs1_encoder_copyHRI(ctx, (void*)buf, needed - 1);
	TEST_CHECK(buf[0] == '\0');

	gs1_encoder_free(ctx);

DIAG_POP

}


#endif  /* UNIT_TESTS */
