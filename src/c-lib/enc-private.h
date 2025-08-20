/**
 * GS1 Barcode Syntax Engine
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

#ifndef ENC_PRIVATE_H
#define ENC_PRIVATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "gs1encoders.h"


// Implementation limits that can be changed
#define MAX_FNAME	120	// Maximum filename
#define MAX_DATA	8191	// Maximum input buffer size


#ifdef _MSC_VER
#define strtok_r strtok_s
#define ssize_t ptrdiff_t
#endif

#if defined(__GNUC__) || defined(__clang__)
#define __ATTR_CONST __attribute__ ((__const__))
#define __ATTR_PURE __attribute__ ((__pure__))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#elif _MSC_VER
#define __ATTR_CONST __declspec(noalias)
#define __ATTR_PURE
#define likely(x) (x)
#define unlikely(x) (x)
#else
#define __ATTR_CONST
#define __ATTR_PURE
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#if defined(__clang__)
#  define DIAG_PUSH _Pragma("clang diagnostic push")
#  define DIAG_POP _Pragma("clang diagnostic pop")
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(__GNUC__)
#  define DIAG_PUSH _Pragma("GCC diagnostic push")
#  define DIAG_POP _Pragma("GCC diagnostic pop")
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(_MSC_VER)
#  define DIAG_PUSH __pragma(warning(push))
#  define DIAG_POP __pragma(warning(pop))
#  define DIAG_DISABLE_DEPRECATED_DECLARATIONS __pragma(warning(disable: 4996))
#endif


#ifdef GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H
#define xstr(s) str(s)
#define str(s) #s
#include xstr(GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H)
#endif

#ifdef GS1_ENCODERS_CUSTOM_HEAP_MANAGEMENT_H
#define GS1_ENCODERS_MALLOC(sz) GS1_ENCODERS_CUSTOM_MALLOC(sz)
#define GS1_ENCODERS_CALLOC(nm, sz) GS1_ENCODERS_CUSTOM_CALLOC(nm, sz)
#define GS1_ENCODERS_REALLOC(p, sz) GS1_ENCODERS_CUSTOM_REALLOC(p, sz)
#define GS1_ENCODERS_FREE(p) GS1_ENCODERS_CUSTOM_FREE(p)
#else
#define GS1_ENCODERS_MALLOC(sz) malloc(sz)
#define GS1_ENCODERS_CALLOC(nm, sz) calloc(nm, sz)
#define GS1_ENCODERS_REALLOC(p, sz) realloc(p, sz)
#define GS1_ENCODERS_FREE(p) free(p)
#endif


#define GS1_SEARCH_INVALID   (-2)
#define GS1_SEARCH_NOT_FOUND (-1)

#define SIZEOF_ARRAY(x) (sizeof(x) / sizeof(x[0]))


#include "ai.h"


/*
 *  May be stabilised as part of the public API in the future.
 *
 */
typedef enum {
	gs1_encoder_eNO_ERROR = 0,
	gs1_encoder_eAI_TABLE_BROKEN_PREFIXES_DIFFER_IN_LENGTH,
	gs1_encoder_eAI_DATA_IS_EMPTY,
	gs1_encoder_eAI_DATA_HAS_INCORRECT_LENGTH,
	gs1_encoder_eAI_LINTER_ERROR,
	gs1_encoder_eAI_VALUE_IS_TOO_SHORT,
	gs1_encoder_eAI_VALUE_IS_TOO_LONG,
	gs1_encoder_eAI_CONTAINS_ILLEGAL_CARAT_CHARACTER,
	gs1_encoder_eAI_UNRECOGNISED,
	gs1_encoder_eTOO_MANY_AIS,
	gs1_encoder_eAI_PARSE_FAILED,
	gs1_encoder_eMISSING_FNC1_IN_FIRST_POSITION,
	gs1_encoder_eAI_DATA_EMPTY,
	gs1_encoder_eNO_AI_FOR_PREFIX,
	gs1_encoder_eAI_DATA_IS_TOO_LONG,
	gs1_encoder_eINVALID_AI_PAIRS,
	gs1_encoder_eREQUIRED_AIS_NOT_SATISFIED,
	gs1_encoder_eINSTANCES_OF_AI_HAVE_DIFFERENT_VALUES,
	gs1_encoder_eSERIAL_NOT_PRESENT,
	gs1_encoder_eFAILED_TO_REALLOC_FOR_KEY_QUALIFIERS,
	gs1_encoder_eFAILED_TO_MALLOC_FOR_KEY_QUALIFIERS,
	gs1_encoder_eURI_CONTAINS_ILLEGAL_CHARACTERS,
	gs1_encoder_eURI_CONTAINS_ILLEGAL_SCHEME,
	gs1_encoder_eURI_MISSING_DOMAIN_AND_PATH_INFO,
	gs1_encoder_eNO_GS1_DL_KEYS_FOUND_IN_PATH_INFO,
	gs1_encoder_eAI_VALUE_PATH_ELEMENT_IS_EMPTY,
	gs1_encoder_eDECODED_AI_FROM_DL_PATH_INFO_CONTAINS_ILLEGAL_NULL,
	gs1_encoder_eUNKNOWN_AI_IN_QUERY_PARAMS,
	gs1_encoder_eAI_VALUE_QUERY_ELEMENT_IN_EMPTY,
	gs1_encoder_eDECODED_AI_VALUE_FROM_QUERY_PARAMS_CONTAINS_ILLEGAL_NULL,
	gs1_encoder_eINVALID_KEY_QUALIFIER_SEQUENCE,
	gs1_encoder_eDUPLICATE_AI,
	gs1_encoder_eAI_IS_NOT_VALID_DATA_ATTRIBUTE,
	gs1_encoder_eAI_SHOULD_BE_IN_PATH_INFO,
	gs1_encoder_eDL_URI_PARSE_FAILED,
	gs1_encoder_eCANNOT_CREATE_DL_URI_WITHOUT_PRIMARY_KEY_AI,
	gs1_encoder_eUNKNOWN_SYMBOLOGY,
	gs1_encoder_eUNKNOWN_VALIDATION,
	gs1_encoder_eVALIDATION_CANNOT_BE_AMENDED,
	gs1_encoder_eDATA_TOO_LONG,
	gs1_encoder_ePRIMARY_DATA_MUST_BE_N_DIGITS_WITHOUT_CHECK_DIGIT,
	gs1_encoder_ePRIMARY_DATA_MUST_BE_N_DIGITS,
	gs1_encoder_ePRIMARY_DATA_MUST_BE_ALL_DIGITS,
	gs1_encoder_ePRIMARY_DATA_CHECK_DIGIT_IS_INCORRECT,
	gs1_encoder_ePRIMARY_DATA_IS_TOO_LARGE,
	gs1_encoder_eMISSING_SYMBOLOGY_IDENTIFIER,
	gs1_encoder_eUNSUPPORTED_SYMBOLOGY_IDENTIFIER,
	gs1_encoder_ePRIMARY_SCAN_DATA_IS_TOO_SHORT,
	gs1_encoder_ePRIMARY_MESSAGE_IS_TOO_LONG,
	gs1_encoder_ePRIMARY_MESSAGE_MAY_ONLY_CONTAIN_DIGITS,
	gs1_encoder_ePRIMARY_MESSAGE_CHECK_DIGIT_IS_INCORRECT,
	gs1_encoder_eSCAN_DATA_CONTAINS_ILLEGAL_CARAT,
	gs1_encoder_eFAILED_TO_PROCESS_SCAN_DATA,
	gs1_encoder_eFORMAT_SPEC_FOR_OPT_COMPONENT_MISSING_RT_SQ_BRACKET,
	gs1_encoder_eUNKNOWN_CHARACTER_SET,
	gs1_encoder_eFORMAT_SPEC_TOO_SHORT,
	gs1_encoder_eAI_LENGTH_TOO_LONG,
	gs1_encoder_eAI_LENGTH_IS_NOT_A_NUMBER,
	gs1_encoder_eUNRECOGNISED_FORMAT_SPECIFICATION,
	gs1_encoder_eNUMBER_OF_LINTERS_EXCEEDS_IMPL_LIMIT,
	gs1_encoder_eUNKNOWN_LINTER,
	gs1_encoder_eENTRY_TOO_LONG,
	gs1_encoder_eSYNTAX_DICTIONARY_CAPACITY_TOO_SMALL,
	gs1_encoder_eAI_RANGE_HAS_WRONG_WIDTH,
	gs1_encoder_eAIS_IN_RANGE_MUST_HAVE_EQUAL_WIDTH,
	gs1_encoder_eAIS_MUST_BE_NUMERIC,
	gs1_encoder_eAI_RANGE_PARTS_MAY_ONLY_DIFFER_IN_LAST_DIGIT,
	gs1_encoder_eAI_RANGE_END_MUST_EXCEED_RANGE_START,
	gs1_encoder_eAI_HAS_WRONG_WIDTH,
	gs1_encoder_eAI_MUST_BE_NUMBERIC,
	gs1_encoder_eTRUNCATED_AFTER_AI,
	gs1_encoder_eTRUNCATED_AFTER_FLAGS,
	gs1_encoder_eNUMBER_OF_AI_COMPONENTS_EXCEEDS_IMPL,
	gs1_encoder_eAI_IS_MISSING_COMPONENTS,
	gs1_encoder_eONLY_FINAL_COMPONENT_MAY_HAVE_VARIABLE_LENGTH,
	gs1_encoder_eMANDATORY_COMPONENT_CANNOT_FOLLOW_OPTIONAL_COMPONENTS,
	gs1_encoder_eATTRIBUTE_NAME_REQUIRED_ON_LHS_OF_ASSIGNMENT,
	gs1_encoder_eATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS,
	gs1_encoder_eATTRIBUTE_VALUE_CONTAINS_ILLEGAL_CHARACTERS,
	gs1_encoder_eATTRIBUTE_VALUE_REQUIRED_ON_RHS_OF_ASSIGNMENT,
	gs1_encoder_eSINGLETON_ATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS,
	gs1_encoder_eATTRIBUTES_TOO_LONG,
	gs1_encoder_eFAILED_TO_ALLOCATE_MEMORY_FOR_ATTRS,
	gs1_encoder_eTITLE_CONTAINS_ILLEGAL_CHARACTERS,
	gs1_encoder_eFAILED_TO_ALLOCATE_MEMORY_FOR_TITLE,
	gs1_encoder_eFAILED_TO_ALLOCATE_AI_TABLE,
	gs1_encoder_eCANNOT_READ_FILE,
	gs1_encoder_eSYNTAX_DICTIONARY_LINE_EXCEEDS_IMPL,
	gs1_encoder_eSYNTAX_DICTIONARY_LINE_ERROR,
	gs1_encoder_eDOMAIN_CONTAINS_ILLEGAL_CHARACTERS,
	__GS1_ENCODERS_NUM_ERRS
} gs1_encoder_err_t;


struct gs1_encoder {

	gs1_encoder_symbologies_t sym;		// Symbology type

	bool addCheckDigit;			// For EAN/UPC and RSS-14/Lim, calculated if true, otherwise validated
	bool permitUnknownAIs;			// Extract AIs that are not in our AI table during AI element string and DL URI parsing
	bool permitZeroSuppressedGTINinDLuris;	// Whether to permit a path component GTIN value to be in GTIN-{8,12,13} format
	bool permitConvenienceAlphas;		// Whether to permit convenience alphas (deprecated, so no API)
	bool includeDataTitlesInHRI;		// Whether to include the Data Titles in HRI string output

	char errMsg[512];			// The translated error message
	gs1_encoder_err_t err;			// The error
	gs1_lint_err_t linterErr;		// Error returned by a linter
	char linterErrMarkup[512];

	char dataStr[MAX_DATA+1];		// Input data buffer passed to the encoders
	char dlAIbuffer[MAX_DATA+1];		// Populated with unbracketed AI string extracted from DL input
	char outStr[2*MAX_DATA+1];		// Buffer to return formatted data
	char *outHRI[MAX_AIS];			// Array of AI element string for HRI printing

	bool localAlloc;			// True if we malloc()ed this struct
	FILE *outfp;

	struct aiEntry *aiTable;		// Pointer to the AI table
	size_t aiTableEntries;			// Number of entries in the AI table
	bool aiTableIsDynamic;			// True if the AI table is loaded from the Syntax Dictionary

	struct aiValue aiData[MAX_AIS];		// List of AI components
	int numAIs;
	struct aiValue *sortedAIs[MAX_AIS];		// Sorted pointers to aiData entries
	int numSortedAIs;				// Number of entries in sortedAIs

	struct validationEntry validationTable[gs1_encoder_vNUMVALIDATIONS];
						// Table of all global validation functions

	uint8_t aiLengthByPrefix[100];		// AI length by two-digit prefix

	char** dlKeyQualifiers;			// List of valid DL key qualifier association strings
	int numDLkeyQualifiers;			// Number of dlKeyQualifiers strings

};


/*
 *  Utility functions
 *
 */
bool gs1_allDigits(const uint8_t *str, size_t len);

char* gs1_strdup_alloc(const char *s);

ssize_t gs1_binarySearch(const void* needle, const void* haystack, const size_t haystack_size,
			 int (*compare)(const void* key, const void* element, const size_t index),
			 bool (*validate)(const void* key, const void* element, const size_t index));


#ifdef UNIT_TESTS

void test_api_getVersion(void);
void test_api_instanceSize(void);
void test_api_init(void);
void test_api_defaults(void);
void test_api_sym(void);
void test_api_addCheckDigit(void);
void test_api_permitUnknownAIs(void);
void test_api_permitZeroSuppressedGTINinDLuris(void);
void test_api_validateAIassociations(void);
void test_api_validations(void);
void test_api_dataStr(void);
void test_api_getAIdataStr(void);
void test_api_getScanData(void);
void test_api_setScanData(void);
void test_api_getHRI(void);
void test_api_copyHRI(void);
void test_api_getDLignoredQueryParams(void);
void test_api_copyDLignoredQueryParams(void);

#endif


#endif /* ENC_PRIVATE_H */
