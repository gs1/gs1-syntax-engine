/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2025 GS1 AISBL.
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

/**
 * @file gs1syntaxdictionary.h
 *
 * @brief This header contains the declarations for the reference linter
 * functions of type ::gs1_linter_t referred to in the [GS1 Barcode Syntax
 * Dictionary](https://ref.gs1.org/tools/gs1-barcode-syntax-resource/syntax-dictionary/),
 * as well as the list of all possible return codes with a description of the
 * indicated error conditions.
 *
 * The reference linter return codes of type ::gs1_lint_err_t are described
 * below.
 *
 * The functional descriptions of the reference linters are available from the
 * table of linters on the [main page](index.html).
 *
 */

#ifndef GS1_SYNTAXDICTIONARY_H
#define GS1_SYNTAXDICTIONARY_H

/// \cond
#include <stddef.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
/// \endcond


#ifdef __EMSCRIPTEN__
#  define GS1_SYNTAX_DICTIONARY_API EMSCRIPTEN_KEEPALIVE
#elif _WIN32
#  define GS1_SYNTAX_DICTIONARY_API __declspec(dllexport)
#elif __GNUC__ >= 4
#  define GS1_SYNTAX_DICTIONARY_API __attribute__((visibility ("default")))
#else
#  define GS1_SYNTAX_DICTIONARY_API
#endif


#if defined(__GNUC__) || defined(__clang__)
#  define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#  define DEPRECATED __declspec(deprecated)
#else
#  define DEPRECATED
#endif


/**
 * @brief Linter return codes other than #GS1_LINTER_OK indicate an error
 * condition.
 *
 * @note Existing values in this enumeration will remain stable over time.
 *
 */
typedef enum
{
	GS1_LINTER_OK = 0,						///< No issues were detected by the linter.
	GS1_LINTER_NON_DIGIT_CHARACTER,					///< A non-digit character was found where a digit is expected.
	GS1_LINTER_INVALID_CSET82_CHARACTER,				///< A non-CSET 82 character was found where a CSET 82 character is expected.
	GS1_LINTER_INVALID_CSET39_CHARACTER,				///< A non-CSET 39 character was found where a CSET 39 character is expected.
	GS1_LINTER_INVALID_CSET32_CHARACTER,				///< A non-CSET 32 character was found where a CSET 32 character is expected.
	GS1_LINTER_INCORRECT_CHECK_DIGIT,				///< The numeric check digit is incorrect.
	GS1_LINTER_TOO_SHORT_FOR_CHECK_DIGIT,				///< The component is too short to perform a numeric check digit calculation.
	GS1_LINTER_INCORRECT_CHECK_PAIR,				///< The alphanumeric check-character pair are incorrect.
	GS1_LINTER_TOO_SHORT_FOR_CHECK_PAIR,				///< The component is too short to perform an alphanumeric check character pair calculation.
	GS1_LINTER_TOO_LONG_FOR_CHECK_PAIR_IMPLEMENTATION,		///< The component is too long to perform an alphanumeric check character pair calculation.
	GS1_LINTER_GCP_DATASOURCE_OFFLINE,				///< The data source for GCP lookups is offline.
	GS1_LINTER_UNUSED_6,
	GS1_LINTER_INVALID_GCP_PREFIX,					///< The GS1 Company Prefix is invalid.
	GS1_LINTER_IMPORTER_IDX_MUST_BE_ONE_CHARACTER,			///< The Importer Index must be a single character.
	GS1_LINTER_INVALID_IMPORT_IDX_CHARACTER,			///< The Importer Index is an invalid character.
	GS1_LINTER_ILLEGAL_ZERO_VALUE,					///< A non-zero value is required.
	GS1_LINTER_NOT_ZERO,						///< A zero value is required.
	GS1_LINTER_ILLEGAL_ZERO_PREFIX,					///< A zero prefix is not permitted.
	GS1_LINTER_NOT_ZERO_OR_ONE,					///< A "0" or "1" is required.
	GS1_LINTER_INVALID_WINDING_DIRECTION,				///< The winding direction must be either "0", "1" or "9".
	GS1_LINTER_NOT_ISO3166,						///< A valid ISO 3166 three-digit country code is required.
	GS1_LINTER_NOT_ISO3166_OR_999,					///< A valid ISO 3166 three-digit country code or "999" is required.
	GS1_LINTER_NOT_ISO3166_ALPHA2,					///< A valid ISO 3166 two-character country code is required.
	GS1_LINTER_NOT_ISO4217,						///< A valid ISO 4217 three-digit currency code is required.
	GS1_LINTER_IBAN_TOO_SHORT,					///< The IBAN is too short.
	GS1_LINTER_INVALID_IBAN_CHARACTER,				///< The IBAN contains an invalid character.
	GS1_LINTER_ILLEGAL_IBAN_COUNTRY_CODE,				///< The IBAN must start with a valid ISO 3166 two-character country code.
	GS1_LINTER_INCORRECT_IBAN_CHECKSUM,				///< The IBAN is invalid since the check characters are incorrect.
	GS1_LINTER_DATE_TOO_SHORT,					///< The date is too short for YYMMDD format.
	GS1_LINTER_DATE_TOO_LONG,					///< The date is too long for YYMMDD format.
	GS1_LINTER_UNUSED_2,
	GS1_LINTER_UNUSED_3,
	GS1_LINTER_UNUSED_5,
	GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT,				///< The hour with minute is too short for HHMI format.
	GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG,				///< The hour with minute is too long for HHMI format.
	GS1_LINTER_UNUSED_4,
	GS1_LINTER_ILLEGAL_MONTH,					///< The date contains an illegal month of the year.
	GS1_LINTER_ILLEGAL_DAY,						///< The date contains an illegal day of the month.
	GS1_LINTER_ILLEGAL_HOUR,					///< The time contains an illegal hour.
	GS1_LINTER_ILLEGAL_MINUTE,					///< The time contains an illegal minute.
	GS1_LINTER_ILLEGAL_SECOND,					///< The time contains an illegal seconds.
	GS1_LINTER_INVALID_LENGTH_FOR_PIECE_OF_TOTAL,			///< The piece with total must have an even length, having equal-length components.
	GS1_LINTER_ZERO_PIECE_NUMBER,					///< The piece number must not have a value of zero.
	GS1_LINTER_ZERO_TOTAL_PIECES,					///< The piece total must not have a value of zero.
	GS1_LINTER_PIECE_NUMBER_EXCEEDS_TOTAL,				///< The piece number must not exceed the piece total.
	GS1_LINTER_INVALID_PERCENT_SEQUENCE,				///< The input contains an invalid percent hex-encoding "%hh" sequence.
	GS1_LINTER_COUPON_MISSING_FORMAT_CODE,				///< The coupon's Format Code is missing.
	GS1_LINTER_COUPON_INVALID_FORMAT_CODE,				///< The coupon's Format Code must be "0" or "1".
	GS1_LINTER_COUPON_MISSING_FUNDER_VLI,				///< The coupon's Funder VLI is missing.
	GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH,			///< The coupon's Funder VLI must be "0" to "6".
	GS1_LINTER_COUPON_TRUNCATED_FUNDER,				///< The coupon's Funder is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE,				///< The coupon's Offer Code is shorter than the required six digits.
	GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI,			///< The coupon's Serial Number VLI is missing.
	GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER,			///< The coupon's Serial Number is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_GCP_VLI,				///< The coupon's primary GS1 Company Prefix VLI is missing.
	GS1_LINTER_COUPON_INVALID_GCP_LENGTH,				///< The coupon's primary GS1 Company Prefix VLI must be "0" to "6".
	GS1_LINTER_COUPON_TRUNCATED_GCP,				///< The coupon's primary GS1 Company Prefix is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_SAVE_VALUE_VLI,			///< The coupon's Save Value VLI is missing.
	GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH,			///< The coupon's Save Value VLI must be "1" to "5".
	GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,				///< The coupon's Save Value is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI,		///< The coupon's primary purchase Requirement VLI is missing.
	GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH,	///< The coupon's primary purchase Requirement VLI must be "1" to "5".
	GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,		///< The coupon's primary purchase Requirement is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE,	///< The coupon's primary purchase Requirement Code is missing.
	GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE,	///< The coupon's primary purchase Requirement Code must be "0" to "4" or "9".
	GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE,		///< The coupon's primary purchase Family Code is shorter than the required three digits.
	GS1_LINTER_COUPON_MISSING_ADDITIONAL_PURCHASE_RULES_CODE,	///< The coupon's Additional Purchase Rules Code is missing.
	GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE,	///< The coupon's Additional Purchase Rules Code must be "0" to "3".
	GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI,		///< The coupon's second purchase Requirement VLI is missing.
	GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH,	///< The coupon's second purchase Requirement VLI must be "1" to "5".
	GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,		///< The coupon's second purchase Requirement is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE,	///< The coupon's second purchase Requirement Code is missing.
	GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE,	///< The coupon's second purchase Requirement Code must be "0" to "4" or "9".
	GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE,		///< The coupon's second purchase Family Code is shorter than the required three digits.
	GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_GCP_VLI,			///< The coupon's second purchase GS1 Company Prefix VLI is missing.
	GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_GCP_LENGTH,		///< The coupon's second purchase GS1 Company Prefix VLI must be "0" to "6" or "9".
	GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP,			///< The coupon's second purchase GS1 Company Prefix is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_VLI,		///< The coupon's third purchase Requirement VLI is missing.
	GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH,	///< The coupon's third purchase Requirement VLI must be "1" to "5".
	GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,		///< The coupon's third purchase Requirement is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE,	///< The coupon's third purchase Requirement Code is missing.
	GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE,	///< The coupon's third purchase Requirement Code must be "0" to "4" or "9".
	GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE,		///< The coupon's third purchase Family Code is shorter than the required three digits.
	GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_GCP_VLI,			///< The coupon's third purchase GS1 Company Prefix VLI is missing.
	GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_GCP_LENGTH,		///< The coupon's third purchase GS1 Company Prefix VLI must be "0" to "6" or "9".
	GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP,			///< The coupon's third purchase GS1 Company Prefix is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE,		///< The coupon's expiration date is too short for YYMMDD format.
	GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE,			///< The coupon's expiration date is invalid.
	GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE,			///< The coupon's start date is too short to YYMMDD format.
	GS1_LINTER_COUPON_INVALID_START_DATE,				///< The coupon's start date is invalid.
	GS1_LINTER_COUPON_EXPIRATION_BEFORE_START,			///< The coupon's expiration date preceed the start date.
	GS1_LINTER_COUPON_MISSING_RETAILER_GCP_OR_GLN_VLI,		///< The coupon's Retailer GCP/GLN VLI is missing.
	GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH,		///< The coupon's Retailer GCP/GLN VLI must be "1" to "7".
	GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN,		///< The coupon's Retailer GCP/GLN is shorter than what is indicated by its VLI.
	GS1_LINTER_COUPON_MISSING_SAVE_VALUE_CODE,			///< The coupon's Save Value Code is missing.
	GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,			///< The coupon's Save Value Code must be "0", "1", "2", "5" or "6".
	GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM,		///< The coupon's Save Value Applies to Item is missing.
	GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM,		///< The coupon's Save Value Applies to Item must be "0" to "2".
	GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG,			///< The coupon's Store Coupon Flag is missing.
	GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG,			///< The coupon's Don't Multiply Flag is missing.
	GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG,			///< The coupon's Don't Multiply Flag must be "0" or "1".
	GS1_LINTER_COUPON_EXCESS_DATA,					///< The coupon contains excess data after the recognised optional fields.
	GS1_LINTER_UNUSED_1,
	GS1_LINTER_INVALID_LATITUDE,					///< The latitude is outside of the range "0000000000" to "1800000000".
	GS1_LINTER_INVALID_LONGITUDE,					///< The longitude is outside of the range "0000000000" to "3600000000".
	GS1_LINTER_INVALID_MEDIA_TYPE,					///< A valid AIDC media type is required.
	GS1_LINTER_LATITUDE_INVALID_LENGTH,				///< The latitude value must be 10 digits.
	GS1_LINTER_LONGITUDE_INVALID_LENGTH,				///< The longitude value must be 10 digits.
	GS1_LINTER_INVALID_CSET64_CHARACTER,				///< A non-CSET 64 character was found where a CSET 64 character is expected.
	GS1_LINTER_INVALID_CSET64_PADDING,				///< Incorrect number of CSET 64 pad characters.
	GS1_LINTER_NOT_HYPHEN,						///< Only hyphens are permitted.
	GS1_LINTER_INVALID_BIOLOGICAL_SEX_CODE,				///< A valid ISO/IEC 5218 biological sex code required.
	GS1_LINTER_POSITION_IN_SEQUENCE_MALFORMED,			///< The data must have the format "<pos>/<end>".
	GS1_LINTER_POSITION_EXCEEDS_END,				///< The position number must not exceed the end number.
	GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER,			///< A non-digit character is required
	GS1_LINTER_HOUR_TOO_SHORT,					///< The hour is too short for HH format.
	GS1_LINTER_HOUR_TOO_LONG,					///< The hour is too long for HH format.
	GS1_LINTER_MINUTE_TOO_SHORT,					///< The minute is too short for MI format.
	GS1_LINTER_MINUTE_TOO_LONG,					///< The minute is too long for MI format.
	GS1_LINTER_SECOND_TOO_SHORT,					///< The second is too short for SS format.
	GS1_LINTER_SECOND_TOO_LONG,					///< The second is too long for SS format.
	GS1_LINTER_INVALID_PACKAGE_TYPE,				///< A valid PackageTypeCode is required.
	GS1_LINTER_TOO_SHORT_FOR_GCP,					///< The component is shorter than the minimum length GS1 Company Prefix.
	GS1_LINTER_IBAN_TOO_LONG,					///< The IBAN is too long.
	__GS1_LINTER_NUM_ERRS						//  Keep this as the last element which captures the size of this enumeration.
} gs1_lint_err_t;


#ifdef GS1_LINTER_ERR_STR_EN
#ifdef __EMSCRIPTEN__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#endif
GS1_SYNTAX_DICTIONARY_API extern const char *gs1_lint_err_str[];
#ifdef __EMSCRIPTEN__
#pragma clang diagnostic pop
#endif
#endif


/**
 * @brief Type specification for all linter functions.
 *
 */
typedef gs1_lint_err_t (*gs1_linter_t)(const char *data, size_t *err_pos, size_t *err_len);


#ifdef __cplusplus
extern "C" {
#endif

GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_couponcode(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_couponposoffer(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset39(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset64(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_cset82(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csetnumeric(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csum(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csumalpha(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_gcppos1(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_gcppos2(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hasnondigit(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hh(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hhmi(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hyphen(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iban(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_importeridx(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso3166(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso3166999(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso3166alpha2(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso4217(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso5218(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_latitude(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_longitude(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mediatype(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mi(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_nonzero(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_nozeroprefix(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_packagetype(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_pieceoftotal(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_posinseqslash(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_pcenc(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_ss(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_winding(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yesno(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yymmd0(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yymmdd(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yyyymmd0(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yyyymmdd(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_zero(const char *data, size_t *err_pos, size_t *err_len);

GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_hhmm(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_iso3166list(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_key(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_keyoff1(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_mmoptss(const char *data, size_t *err_pos, size_t *err_len);
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_yymmddhh(const char *data, size_t *err_pos, size_t *err_len);

GS1_SYNTAX_DICTIONARY_API gs1_linter_t gs1_linter_from_name(const char *name);

#ifdef __cplusplus
}
#endif


#endif  /* GS1_SYNTAXDICTIONARY_H */
