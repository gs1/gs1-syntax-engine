/*
 * GS1 Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_couponcode.c
 *
 * @brief The `couponcode` linter ensures that the data conforms to the
 * original North American Coupon Code (NACC) specification, as carried in AI
 * (8110).
 *
 * @remark Refer to the document "North American Coupon Application
 * Guideline Using GS1 DataBar Expanded Symbols" for a description of the GS1
 * U.S. coupon code data content.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to the North American Coupon
 * Code (NACC) specification, as carried in AI (8110).
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_COUPON_MISSING_GCP_VLI if the data is missing a primary
 *         GCP VLI.
 * @return #GS1_LINTER_COUPON_INVALID_GCP_LENGTH if the data contains a
 *         primary GCP with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_GCP if the data contains a primary GCP
 *         that is shorter than is indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_SAVE_VALUE_VLI if the data is missing a
 *         Save Value VLI.
 * @return #GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH if the data contains a
 *         Save Value VLI with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE if the data comtains a Save
 *         Value that is shorter than is indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI if the data
 *         is missing a primary purchace Requirement VLI.
 * @return #GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH if the
 *         data contains a primary purchase Requirement VLI with an invalid
 *         length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT if the data
 *         comtains a primary purchase Requirement that is shorter than is
 *         indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE if the
 *         data is missing a primary purchase Requirement Code.
 * @return #GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE if the
 *         data contains a primary purchase Requirement Code that is too short.
 * @return #GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE if the data
 *         contains a primary purchase Family Code that is too short.
 * @return #GS1_LINTER_COUPON_MISSING_ADDITIONAL_PURCHASE_RULES_CODE
 *         if the data contains an optional field 1 that is missing an
 *         Additional Purchase Rules Code.
 * @return #GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE
 *         if the data contains an optional field 1 whose Additional Purchase
 *         Rules Code is invalid.
 * @return #GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI if
 *         the data contains an optional field 1 that is missing a second
 *         purchase Requirement VLI.
 * @return #GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH
 *         if the data contains an optional field 1 with a second purchase
 *         Requirement VLI with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT if
 *         the data contains an optional field 1 whose second purchase Requirement
 *         Code is shorter than is indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE
 *         if the data contains an optional field 1 that is missing a second
 *         purchase Requirement Code.
 * @return #GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE
 *         if the data contains an optional field 1 whose second purchase
 *         Requirement Code is invalid.
 * @return #GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE if
 *         the data contains an optional field 1 whose second purchase Family
 *         Code is too short.
 * @return #GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_GCP_VLI if the data
 *         contains an optional field 1 that is missing a second purchase GCP
 *         VLI.
 * @return #GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_GCP_LENGTH if the
 *         data contains an optional field 1 with a second purchase GCP VLI
 *         with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP if the data
 *         contains an optional field 1 with a second purchase GCP that is
 *         shorter than indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_VLI if
 *         the data contains an optional field 2 that is missing a third
 *         purchase Requirement VLI.
 * @return #GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH
 *         if the data contains an optional field 2 with a third purchase
 *         Requirement VLI with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT if
 *         the data contains an optional field 2 whose third purchase Requirement
 *         Code is shorter than is indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE
 *         if the data contains an optional field 2 that is missing a third
 *         purchase Requirement Code.
 * @return #GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE
 *         if the data contains an optional field 2 whose third purchase
 *         Requirement Code is invalid.
 * @return #GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE if
 *         the data contains an optional field 2 whose third purchase Family
 *         Code is too short.
 * @return #GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_GCP_VLI if the data
 *         contains an optional field 2 that is missing a third purchase GCP
 *         VLI.
 * @return #GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_GCP_LENGTH if the
 *         data contains an optional field 2 with a third purchase GCP VLI
 *         with an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP if the data
 *         contains an optional field 2 with a third purchase GCP that is
 *         shorter than indicated by its VLI.
 * @return #GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE if the
 *         data contains an optional field 3 whose expiration date is too
 *         short.
 * @return #GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE if the data
 *         contains an optional field 3 whose expiration date is invalid.
 * @return #GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE if the
 *         data contains an optional field 4 whose start date is too short.
 * @return #GS1_LINTER_COUPON_INVALID_START_DATE if the data
 *         contains an optional field 4 whose start date is invalid.
 * @return #GS1_LINTER_COUPON_EXPIRATION_BEFORE_START if the data contains an
 *         optional field 3 and an optional field 4 where the expiration date
 *         is prior to the start date.
 * @return #GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI if the data
 *         contains an optional field 5 that is missing the Serial Number VLI.
 * @return #GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER if the data
 *         contains an optional field 5 whose Serial Number is shorter than
 *         indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_RETAILER_GCP_OR_GLN_VLI f the
 *         data contains an optional field 6 that is missing the Retailer
 *         GCP/GLN VLI.
 * @return #GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH if the
 *         data contains an optional field 6 with a Retailer GCP/GLN VLI with
 *         an invalid length.
 * @return #GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN if the data
 *         contains an optional field 6 whose Retailer GCP/GLN is shorter than
 *         indicated by its VLI.
 * @return #GS1_LINTER_COUPON_MISSING_SAVE_VALUE_CODE if the data
 *         contains an optional field 9 that is missing the Save Value Code.
 * @return #GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE if the data
 *         contains an optional field 9 whose Save Value Code is invalid.
 * @return #GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM if
 *         the data contains an optional field 9 that is missing the Save Value
 *         Applies to Item value.
 * @return #GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM if
 *         the data contains an optional field 9 whose Save Value Applies to
 *         Item value is invalid.
 * @return #GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG if the data
 *         contains an optional field 9 that is missing the Store Coupon Flag.
 * @return #GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG if the data
 *         contains an optional field 9 that is missing the Don't Multiply
 *         Flag.
 * @return #GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG if the data
 *         contains an optional field 9 whose Don't Multiply Flag is invalid.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_couponcode(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	gs1_lint_err_t ret;
	size_t pos;
	int vli;
	const char *p, *q;
	char expiry_date[7] = {0};
	int expiry_set = 0;
	char gcp[14] = {0};

	assert(data);

	/*
	 * Data must consist of all digits.
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != strlen(data)) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_NON_DIGIT_CHARACTER;
	}

	p = data;
	q = data + strlen(data);


	/*
	 * Validate that the GCP follows its VLI and has the corresponding
	 * length (plus 6).
	 *
	 * Valid GCP VLIs are "0" to "6".
	 *
	 */
	if (p == q) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = (size_t)(q - data);
		return GS1_LINTER_COUPON_MISSING_GCP_VLI;
	}
	if (*p > '6') {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = 1;
		return GS1_LINTER_COUPON_INVALID_GCP_LENGTH;
	}
	vli = *p - '0' + 6;

	if (q - ++p < vli) {
		if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
		if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
		return GS1_LINTER_COUPON_TRUNCATED_GCP;
	}

	/*
	 * Validate the GCP with the "key" linter.
	 *
	 */
	strncpy(gcp, p, (size_t)vli);
	ret = gs1_lint_key(gcp, err_pos, err_len);

	assert (ret == GS1_LINTER_OK ||
		ret == GS1_LINTER_INVALID_GCP_PREFIX ||
		ret == GS1_LINTER_GCP_DATASOURCE_OFFLINE);

	if (ret != GS1_LINTER_OK) {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = (size_t)vli;
		return ret;
	}

	p += vli;


	/*
	 * Validate the existence of the six digit Offer Code.
	 *
	 */
	if (q - p < 6) {
		if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
		if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
		return GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE;
	}

	p += 6;


	/*
	 * Validate that the Save Value follows its VLI and has the
	 * corresponding length.
	 *
	 * Valid Save Value VLIs are "1" to "5".
	 *
	 */
	if (p == q) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = (size_t)(q - data);
		return GS1_LINTER_COUPON_MISSING_SAVE_VALUE_VLI;
	}
	if (*p < '1' || *p > '5') {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = 1;
		return GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH;
	}
	vli = *p - '0';

	if (q - ++p < vli) {
		if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
		if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
		return GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE;
	}

	p += vli;


	/*
	 * Validate 1st Purchase Requirement follows its VLI and has the
	 * corresponding length.
	 *
	 * Valid 1st Purchase Requirement VLIs are "1" to "5".
	 *
	 */
	if (p == q) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = (size_t)(q - data);
		return GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI;
	}
	if (*p < '1' || *p > '5') {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = 1;
		return GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH;
	}
	vli = *p - '0';

	if (q - ++p < vli) {
		if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
		if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
		return GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT;
	}

	p += vli;


	/*
	 * Validate the single-digit 1st Purchase Requirement Code.
	 *
	 * Valid values for 1st Purchase Requirement Code are "0" to "4" and
	 * "9".
	 *
	 */
	if (p == q) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = (size_t)(q - data);
		return GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE;
	}

	if (*p > '4' && *p != '9') {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = 1;
		return GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE;
	}


	/*
	 * Validate the existence of the three-digit 1st Purchase Family Code.
	 *
	 */
	if (q - ++p < 3) {
		if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
		if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
		return GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE;
	}

	p += 3;


	/*
	 * Optional field 1: Additional rules and 2nd purchase
	 * ===================================================
	 *
	 * This option field is indicated by the value "1".
	 *
	 */
	if (p < q && *p == '1') {

		/*
		 * Validate the single-digit Additional Purchase Rules Code.
		 *
		 * Valid values for the  Additional Purchase Rules Code are "0"
		 * to "3".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_ADDITIONAL_PURCHASE_RULES_CODE;
		}

		if (*p > '3') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE;
		}


		/*
		 * Validate 2nd Purchase Requirement follows its VLI and has
		 * the corresponding length.
		 *
		 * Valid values for the 2nd Purchase Requirement VLI are "1" to
		 * "5".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI;
		}
		if (*p < '1' || *p > '5') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH;
		}
		vli = *p - '0';

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT;
		}

		p += vli;


		/*
		 * Validate the single-digit 2nd Purchase Requirement Code.
		 *
		 * Valid values for the 2nd Purchase Requirement Code are "0"
		 * to "4" and "9".
		 *
		 */
		if (p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE;
		}

		if (*p > '4' && *p != '9') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE;
		}


		/*
		 * Validate the existence of the three-digit 2nd Purchase
		 * Family Code.
		 *
		 */
		if (q - ++p < 3) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE;
		}

		p += 3;


		/*
		 * Validate that the 2nd Purchase GCP follows its VLI and has
		 * the corresponding length (plus 6, unless VLI is "9").
		 *
		 * Valid values for the 2nd Purchase GCP VLI are "0" to "6" and
		 * "9".
		 *
		 */
		if (p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_GCP_VLI;
		}
		if (*p > '6' && *p != '9') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_GCP_LENGTH;
		}
		vli = (*p != '9') ? *p - '0' + 6 : 0;

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP;
		}

		/*
		 * Validate the GCP with the "key" linter.
		 *
		 */
		strncpy(gcp, p, (size_t)vli);
		ret = gs1_lint_key(gcp, err_pos, err_len);

		assert (ret == GS1_LINTER_OK ||
			ret == GS1_LINTER_INVALID_GCP_PREFIX ||
			ret == GS1_LINTER_GCP_DATASOURCE_OFFLINE);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = (size_t)vli;
			return ret;
		}

		p += vli;

	}


	/*
	 * Optional field 2: 3rd purchase
	 * ==============================
	 *
	 * This option field is indicated by the value "2".
	 *
	 */
	if (p < q && *p == '2') {

		/*
		 * Validate 3rd Purchase Requirement follows its VLI and has
		 * the corresponding length.
		 *
		 * Valid values for the 3rd Purchase Requirement VLI are "1" to
		 * "5".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_VLI;
		}
		if (*p < '1' || *p > '5') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH;
		}
		vli = *p - '0';

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT;
		}

		p += vli;


		/*
		 * Validate the single-digit 3nd Purchase Requirement Code.
		 *
		 * Valie values for the 3nd Purchase Requirement Code are "0"
		 * to "4" and "9".
		 *
		 */
		if (p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE;
		}

		if (*p > '4' && *p != '9') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE;
		}


		/*
		 * Validate that existence of the three digit 3rd Purchase
		 * Family Code.
		 *
		 */
		if (q - ++p < 3) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE;
		}

		p += 3;


		/*
		 * Validate that the 3rd Purchase GCP follows its VLI and has
		 * the corresponding length (plus 6, unless VLI is "9").
		 *
		 * Valid values for the 3rd Purchase GCP VLI are "0" to "6" and
		 * "9".
		 *
		 */
		if (p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_GCP_VLI;
		}
		if (*p > '6' && *p != '9') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_GCP_LENGTH;
		}
		vli = (*p != '9') ? *p - '0' + 6 : 0;

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP;
		}

		/*
		 * Validate the GCP with the "key" linter.
		 *
		 */
		strncpy(gcp, p, (size_t)vli);
		ret = gs1_lint_key(gcp, err_pos, err_len);

		assert (ret == GS1_LINTER_OK ||
			ret == GS1_LINTER_INVALID_GCP_PREFIX ||
			ret == GS1_LINTER_GCP_DATASOURCE_OFFLINE);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = (size_t)vli;
			return ret;
		}

		p += vli;

	}


	/*
	 * Optional field 3: Expiration date
	 * =================================
	 *
	 * This option field is indicated by the value "3".
	 *
	 */
	if (p < q && *p == '3') {

		/*
		 * Validate that the expiration date is in YYMMDD format.
		 *
		 */
		if (q - ++p < 6) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE;
		}

		memcpy(expiry_date, p, 6);
		ret = gs1_lint_yymmdd(expiry_date, err_pos, err_len);

		assert(ret == GS1_LINTER_OK ||
		       ret == GS1_LINTER_DATE_TOO_SHORT ||
		       ret == GS1_LINTER_DATE_TOO_LONG ||
		       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
		       ret == GS1_LINTER_ILLEGAL_MONTH ||
		       ret == GS1_LINTER_ILLEGAL_DAY);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 6;
			return GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE;
		}

		p += 6;

		expiry_set = 1;

	}


	/*
	 * Optional field 4: Start date
	 * ============================
	 *
	 * This option field is indicated by the value "4".
	 *
	 */
	if (p < q && *p == '4') {

		char start_date[7] = {0};

		/*
		 * Validate that the start date is in YYMMDD format.
		 *
		 */
		if (q - ++p < 6) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE;
		}

		memcpy(start_date, p, 6);
		ret = gs1_lint_yymmdd(start_date, err_pos, err_len);

		assert(ret == GS1_LINTER_OK ||
		       ret == GS1_LINTER_DATE_TOO_SHORT ||
		       ret == GS1_LINTER_DATE_TOO_LONG ||
		       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
		       ret == GS1_LINTER_ILLEGAL_MONTH ||
		       ret == GS1_LINTER_ILLEGAL_DAY);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 6;
			return GS1_LINTER_COUPON_INVALID_START_DATE;
		}

		/*
		 * If an expiration date has been set, then ensure that it does
		 * not preceed the start date.
		 *
		 */
		if (expiry_set && strcmp(start_date, expiry_date) > 0) {
			if (err_pos) *err_pos = (size_t)(p - data - 8);
			if (err_len) *err_len = 14;
			return GS1_LINTER_COUPON_EXPIRATION_BEFORE_START;
		}

		p += 6;

	}


	/*
	 * Optional field 5: Serial number
	 * ===============================
	 *
	 * This option field is indicated by the value "5".
	 *
	 */
	if (p < q && *p == '5') {

		/*
		 * Validate Serial Number follows its VLI and has the corresponding
		 * length (plus 6).
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI;
		}
		vli = *p - '0' + 6;

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER;
		}

		p += vli;

	}


	/*
	 * Optional field 6: Retailer GCP/GLN
	 * ==================================
	 *
	 * This option field is indicated by the value "6".
	 *
	 */
	if (p < q && *p == '6') {

		/*
		 * Validate that the Retailer GCP/GLN follows its VLI and has
		 * the corresponding length (plus 6).
		 *
		 * Valid values for the Retailer GCP/GLN VLI are "1" to "7".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_RETAILER_GCP_OR_GLN_VLI;
		}
		if (*p < '1' || *p > '7') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH;
		}
		vli = *p - '0' + 6;

		if (q - ++p < vli) {
			if (err_pos) *err_pos = (p == q) ? 0                  : (size_t)(p - data);
			if (err_len) *err_len = (p == q) ? (size_t)(q - data) : (size_t)(q - p);
			return GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN;
		}

		/*
		 * Validate the GCP/GLN with the "key" linter.
		 *
		 */
		strncpy(gcp, p, (size_t)vli);
		ret = gs1_lint_key(gcp, err_pos, err_len);

		assert (ret == GS1_LINTER_OK ||
			ret == GS1_LINTER_INVALID_GCP_PREFIX ||
			ret == GS1_LINTER_GCP_DATASOURCE_OFFLINE);

		if (ret != GS1_LINTER_OK) {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = (size_t)vli;
			return ret;
		}

		p += vli;

	}


	/*
	 * Optional field 9: Miscellaneous
	 * ===============================
	 *
	 * This option field is indicated by the value "9".
	 *
	 */
	if (p < q && *p == '9') {

		/*
		 * Validate the single-digit Save Value Code.
		 *
		 * Valid values for the Save Value Code are "0" to "2" and "5"
		 * to "6".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_SAVE_VALUE_CODE;
		}
		if (*p != '0' && *p != '1' && *p != '2' && *p != '5' && *p != '6') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE;
		}


		/*
		 * Validate the single-digit Save Value Applies to Item value.
		 *
		 * Valid values for Save Value Applies to Item are "0" to "2".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM;
		}
		if (*p > '2') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM;
		}


		/*
		 * Validate the existence of the single-digit Store Coupon
		 * Flag.
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG;
		}


		/*
		 * Validate the single-digit Don't Multiply Flag.
		 *
		 * Valid values for the Don't Multiply Flag are "0" and "1".
		 *
		 */
		if (++p == q) {
			if (err_pos) *err_pos = 0;
			if (err_len) *err_len = (size_t)(q - data);
			return GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG;
		}
		if (*p != '0' && *p != '1') {
			if (err_pos) *err_pos = (size_t)(p - data);
			if (err_len) *err_len = 1;
			return GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG;
		}

		p++;

	}

	/*
	 * Report excess data that has not been handled as an optional field.
	 *
	 */
	if (p != q) {
		if (err_pos) *err_pos = (size_t)(p - data);
		if (err_len) *err_len = (size_t)(q - p);
		return GS1_LINTER_COUPON_EXCESS_DATA;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_couponcode(void)
{

	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "a12345612345611110123", GS1_LINTER_NON_DIGIT_CHARACTER, "*a*12345612345611110123");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012a", GS1_LINTER_NON_DIGIT_CHARACTER, "01234561234561111012*a*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "", GS1_LINTER_COUPON_MISSING_GCP_VLI, "**");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01",     GS1_LINTER_COUPON_TRUNCATED_GCP, "0*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012",    GS1_LINTER_COUPON_TRUNCATED_GCP, "0*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123",   GS1_LINTER_COUPON_TRUNCATED_GCP, "0*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234",  GS1_LINTER_COUPON_TRUNCATED_GCP, "0*1234*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345",           GS1_LINTER_COUPON_TRUNCATED_GCP,        "0*12345*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456",          GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*0123456*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "1123456",          GS1_LINTER_COUPON_TRUNCATED_GCP,        "1*123456*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "11234567",         GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*11234567*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "21234567",         GS1_LINTER_COUPON_TRUNCATED_GCP,        "2*1234567*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "212345678",        GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*212345678*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "312345678",        GS1_LINTER_COUPON_TRUNCATED_GCP,        "3*12345678*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "3123456789",       GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*3123456789*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "4123456789",       GS1_LINTER_COUPON_TRUNCATED_GCP,        "4*123456789*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "41234567890",      GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*41234567890*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "51234567890",      GS1_LINTER_COUPON_TRUNCATED_GCP,        "5*1234567890*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "512345678901",     GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*512345678901*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "612345678901",     GS1_LINTER_COUPON_TRUNCATED_GCP,        "6*12345678901*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "6123456789012",    GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*6123456789012*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "71234567890123",   GS1_LINTER_COUPON_INVALID_GCP_LENGTH, "*7*1234567890123");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "812345678901234",  GS1_LINTER_COUPON_INVALID_GCP_LENGTH, "*8*12345678901234");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "9123456789012345", GS1_LINTER_COUPON_INVALID_GCP_LENGTH, "*9*123456789012345");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561",                GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "0123456*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612",               GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "0123456*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123",              GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "0123456*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234",             GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "0123456*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345",            GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "0123456*12345*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456",           GS1_LINTER_COUPON_MISSING_SAVE_VALUE_VLI, "*0123456123456*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234560",          GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH, "0123456123456*0*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561",          GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,                 "*01234561234561*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611",         GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI, "*012345612345611*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345621",         GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,                 "01234561234562*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456212",        GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI, "*0123456123456212*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456312",        GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,                 "01234561234563*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234563123",       GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI, "*01234561234563123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234564123",       GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,                 "01234561234564*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345641234",      GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI, "*012345612345641234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345651234",      GS1_LINTER_COUPON_TRUNCATED_SAVE_VALUE,                 "01234561234565*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456512345",     GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_VLI, "*0123456123456512345*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234566123456",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH, "0123456123456*6*123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345671234567",   GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH, "0123456123456*7*1234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456812345678",  GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH, "0123456123456*8*12345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234569123456789", GS1_LINTER_COUPON_INVALID_SAVE_VALUE_LENGTH, "0123456123456*9*123456789");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456110",          GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH, "012345612345611*0*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111",          GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,    "*0123456123456111*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111",         GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE, "*01234561234561111*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561121",         GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,    "0123456123456112*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611212",        GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE, "*012345612345611212*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611312",        GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,    "0123456123456113*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456113123",       GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE, "*0123456123456113123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456114123",       GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,    "0123456123456114*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561141234",      GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE, "*01234561234561141234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561151234",      GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_REQUIREMENT,    "0123456123456115*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611512345",     GS1_LINTER_COUPON_MISSING_1ST_PURCHASE_REQUIREMENT_CODE, "*012345612345611512345*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456116123456",    GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH, "012345612345611*6*123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561171234567",   GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH, "012345612345611*7*1234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611812345678",  GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH, "012345612345611*8*12345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456119123456789", GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_LENGTH, "012345612345611*9*123456789");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611110*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611111",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611111*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611112",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611112*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611113",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611113*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611114",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611114*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611115",   GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE, "01234561234561111*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611116",   GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE, "01234561234561111*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611117",   GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE, "01234561234561111*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611118",   GS1_LINTER_COUPON_INVALID_1ST_PURCHASE_REQUIREMENT_CODE, "01234561234561111*8*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611119",   GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "*012345612345611119*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101",  GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "012345612345611110*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012", GS1_LINTER_COUPON_TRUNCATED_1ST_PURCHASE_FAMILY_CODE, "012345612345611110*12*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123");

	/* Invalid optional fields */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101230", GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123*0*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101237", GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101238", GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123*8*");

	/* Optional field 1 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231", GS1_LINTER_COUPON_MISSING_ADDITIONAL_PURCHASE_RULES_CODE, "*0123456123456111101231*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310", GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI, "*01234561234561111012310*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012311", GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI, "*01234561234561111012311*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012312", GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI, "*01234561234561111012312*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012313", GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_VLI, "*01234561234561111012313*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012314", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*4*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012315", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012316", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012317", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012318", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*8*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012319", GS1_LINTER_COUPON_INVALID_ADDITIONAL_PURCHASE_RULES_CODE, "0123456123456111101231*9*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123100",          GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH, "01234561234561111012310*0*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101",          GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,    "*012345612345611110123101*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011",         GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE, "*0123456123456111101231011*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231021",         GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,    "012345612345611110123102*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310212",        GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE, "*01234561234561111012310212*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310312",        GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,    "012345612345611110123103*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123103123",       GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE, "*012345612345611110123103123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123104123",       GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,    "012345612345611110123104*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231041234",      GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE, "*0123456123456111101231041234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231051234",      GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_REQUIREMENT,    "012345612345611110123105*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310512345",     GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_REQUIREMENT_CODE, "*01234561234561111012310512345*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123106123456",    GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH, "01234561234561111012310*6*123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231071234567",   GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH, "01234561234561111012310*7*1234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310812345678",  GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH, "01234561234561111012310*8*12345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123109123456789", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_LENGTH, "01234561234561111012310*9*123456789");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310110*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310111", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310111*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310112", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310112*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310113", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310113*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310114", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310114*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310115", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE, "0123456123456111101231011*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310116", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE, "0123456123456111101231011*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310117", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE, "0123456123456111101231011*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310118", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_REQUIREMENT_CODE, "0123456123456111101231011*8*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310119",   GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "*01234561234561111012310119*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101101",  GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "01234561234561111012310110*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012", GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_FAMILY_CODE, "01234561234561111012310110*12*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123",                GS1_LINTER_COUPON_MISSING_2ND_PURCHASE_GCP_VLI, "*01234561234561111012310110123*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101101230",               GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "*012345612345611110123101101230*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012301",              GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101230*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123012",             GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101230*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101101230123",            GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101230*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012301234",           GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101230*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123012345",          GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101230*12345*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123101101230123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101101231123456",         GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101231*123456*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101231011012311234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012321234567",        GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101232*1234567*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012310110123212345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123312345678",       GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101233*12345678*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123101101233123456789");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123101101234123456789",      GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101234*123456789*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101231011012341234567890");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012351234567890",     GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101235*1234567890*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012310110123512345678901");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123612345678901",    GS1_LINTER_COUPON_TRUNCATED_2ND_PURCHASE_GCP, "012345612345611110123101101236*12345678901*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123101101236123456789012");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012371234567890123",  GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_GCP_LENGTH, "01234561234561111012310110123*7*1234567890123");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012310110123812345678901234", GS1_LINTER_COUPON_INVALID_2ND_PURCHASE_GCP_LENGTH, "01234561234561111012310110123*8*12345678901234");

	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123101101239");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012391",       GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123101101239*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101231011012301234560", GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123101101230123456*0*");

	/* Optional field 2 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232",           GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_VLI, "*0123456123456111101232*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012320",          GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH, "0123456123456111101232*0*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321",          GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,    "*01234561234561111012321*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211",         GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE, "*012345612345611110123211*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123221",         GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,    "01234561234561111012322*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232212",        GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE, "*0123456123456111101232212*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232312",        GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,    "01234561234561111012323*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012323123",       GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE, "*01234561234561111012323123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012324123",       GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,    "01234561234561111012324*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123241234",      GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE, "*012345612345611110123241234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123251234",      GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_REQUIREMENT,    "01234561234561111012325*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232512345",     GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_REQUIREMENT_CODE, "*0123456123456111101232512345*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012326123456",    GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH, "0123456123456111101232*6*123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123271234567",   GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH, "0123456123456111101232*7*1234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232812345678",  GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH, "0123456123456111101232*8*12345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012329123456789", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_LENGTH, "0123456123456111101232*9*123456789");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232110*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232111", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232111*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232112", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232112*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232113", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232113*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232114", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232114*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232115", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE, "012345612345611110123211*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232116", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE, "012345612345611110123211*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232117", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE, "012345612345611110123211*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232118", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_REQUIREMENT_CODE, "012345612345611110123211*8*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232119",   GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "*0123456123456111101232119*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321101",  GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "0123456123456111101232110*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_FAMILY_CODE, "0123456123456111101232110*12*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123", GS1_LINTER_COUPON_MISSING_3RD_PURCHASE_GCP_VLI, "*0123456123456111101232110123*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321101230",            GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "*01234561234561111012321101230*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012301",           GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101230*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123012",          GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101230*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321101230123",         GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101230*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012301234",        GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101230*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123012345",       GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101230*12345*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012321101230123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321101231123456",      GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101231*123456*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123211012311234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012321234567",     GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101232*1234567*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101232110123212345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123312345678",    GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101233*12345678*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012321101233123456789");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012321101234123456789",   GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101234*123456789*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123211012341234567890");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012351234567890",  GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101235*1234567890*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101232110123512345678901");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123612345678901", GS1_LINTER_COUPON_TRUNCATED_3RD_PURCHASE_GCP, "01234561234561111012321101236*12345678901*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012321101236123456789012");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012371234567890123",  GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_GCP_LENGTH, "0123456123456111101232110123*7*1234567890123");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101232110123812345678901234", GS1_LINTER_COUPON_INVALID_3RD_PURCHASE_GCP_LENGTH, "0123456123456111101232110123*8*12345678901234");

	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012321101239");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012391",       GS1_LINTER_COUPON_EXCESS_DATA, "01234561234561111012321101239*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123211012301234560", GS1_LINTER_COUPON_EXCESS_DATA, "01234561234561111012321101230123456*0*");

	/* Optional field 3 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233",        GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "*0123456123456111101233*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012331",       GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "0123456123456111101233*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123312",      GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "0123456123456111101233*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233123",     GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "0123456123456111101233*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012331234",    GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "0123456123456111101233*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123312345",   GS1_LINTER_COUPON_TOO_SHORT_FOR_EXPIRATION_DATE, "0123456123456111101233*12345*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233000606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233250606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233500606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233750606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233990606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200131");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200010",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200010*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233201310",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*201310*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233209910",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*209910*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200131");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200132",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200132*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233000229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233040229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233960229");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200230",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200230*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200331");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200332",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200332*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200430");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200431",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200431*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200531");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200532",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200532*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200630");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200631",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200631*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200731");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200732",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200732*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200831");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200832",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200832*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233200930");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200931",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200931*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233201031");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233201032",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*201032*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233201130");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233201131",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*201131*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101233201231");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233201232",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*201232*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233210229",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*210229*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101233200600",  GS1_LINTER_COUPON_INVALID_EXIPIRATION_DATE, "0123456123456111101233*200600*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012332002290", GS1_LINTER_COUPON_EXCESS_DATA, "0123456123456111101233200229*0*");

	/* Optional field 4 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234",       GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "*0123456123456111101234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012341",      GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "0123456123456111101234*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123412",     GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "0123456123456111101234*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234123",    GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "0123456123456111101234*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012341234",   GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "0123456123456111101234*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123412345",  GS1_LINTER_COUPON_TOO_SHORT_FOR_START_DATE, "0123456123456111101234*12345*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234000606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234250606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234500606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234750606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234990606");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200131");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200010", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200010*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234201310", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*201310*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234209910", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*209910*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200131");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200132", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200132*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234000229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234040229");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234960229");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200230", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200230*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200331");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200332", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200332*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200430");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200431", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200431*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200531");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200532", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200532*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200630");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200631", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200631*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200731");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200732", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200732*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200831");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200832", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200832*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234200930");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200931", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200931*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234201031");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234201032", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*201032*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234201130");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234201131", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*201131*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101234201231");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234201232", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*201232*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234210229", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*210229*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101234200600", GS1_LINTER_COUPON_INVALID_START_DATE, "0123456123456111101234*200600*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012335006064500607", GS1_LINTER_COUPON_EXPIRATION_BEFORE_START, "012345612345611110123*35006064500607*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012335006064500606");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012342002290", GS1_LINTER_COUPON_EXCESS_DATA, "0123456123456111101234200229*0*");

	/* Optional field 5 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235",                GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI, "*0123456123456111101235*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012350",               GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "*01234561234561111012350*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123501",              GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012350*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235012",             GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012350*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012350123",            GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012350*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123501234",           GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012350*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235012345",          GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012350*12345*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012350123456");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012351123456",         GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012351*123456*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123511234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123521234567",        GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012352*1234567*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101235212345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235312345678",       GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012353*12345678*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012353123456789");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012354123456789",      GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012354*123456789*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123541234567890");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123551234567890",     GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012355*1234567890*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101235512345678901");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235612345678901",    GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012356*12345678901*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012356123456789012");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012357123456789012",   GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012357*123456789012*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123571234567890123");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123581234567890123",  GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012358*1234567890123*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101235812345678901234");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101235912345678901234", GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "01234561234561111012359*12345678901234*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012359123456789012345");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123501234560", GS1_LINTER_COUPON_EXCESS_DATA, "01234561234561111012350123456*0*");

	/* Optional field 6 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236",                 GS1_LINTER_COUPON_MISSING_RETAILER_GCP_OR_GLN_VLI, "*0123456123456111101236*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012360",                GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH, "0123456123456111101236*0*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012361",                GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "*01234561234561111012361*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123611",               GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*1*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236112",              GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*12*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012361123",             GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*123*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123611234",            GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236112345",           GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*12345*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012361123456",          GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012361*123456*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123611234567");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123621234567",         GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012362*1234567*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101236212345678");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236312345678",        GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012363*12345678*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012363123456789");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012364123456789",       GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012364*123456789*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123641234567890");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123651234567890",      GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012365*1234567890*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "0123456123456111101236512345678901");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236612345678901",     GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012366*12345678901*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012366123456789012");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012367123456789012",    GS1_LINTER_COUPON_TRUNCATED_RETAILER_GCP_OR_GLN, "01234561234561111012367*123456789012*");
	UNIT_TEST_PASS(gs1_lint_couponcode, "012345612345611110123671234567890123");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236812345678901234",  GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH, "0123456123456111101236*8*12345678901234");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012369123456789012345", GS1_LINTER_COUPON_INVALID_RETAILER_GCP_OR_GLN_LENGTH, "0123456123456111101236*9*123456789012345");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101236112345670", GS1_LINTER_COUPON_EXCESS_DATA, "012345612345611110123611234567*0*");

	/* Optional field 9 */
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239",      GS1_LINTER_COUPON_MISSING_SAVE_VALUE_CODE, "*0123456123456111101239*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390",     GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM, "*01234561234561111012390*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012391",     GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM, "*01234561234561111012391*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012392",     GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM, "*01234561234561111012392*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012393",     GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,            "0123456123456111101239*3*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012394",     GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,            "0123456123456111101239*4*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012395",     GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM, "*01234561234561111012395*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012396",     GS1_LINTER_COUPON_MISSING_SAVE_VALUE_APPLIES_TO_ITEM, "*01234561234561111012396*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012397",     GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,            "0123456123456111101239*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012398",     GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,            "0123456123456111101239*8*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012399",     GS1_LINTER_COUPON_INVALID_SAVE_VALUE_CODE,            "0123456123456111101239*9*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123900",    GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG,          "*012345612345611110123900*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123901",    GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG,          "*012345612345611110123901*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123902",    GS1_LINTER_COUPON_MISSING_STORE_COUPON_FLAG,          "*012345612345611110123902*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123903",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*3*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123904",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*4*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123905",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123906",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123907",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123908",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*8*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123909",    GS1_LINTER_COUPON_INVALID_SAVE_VALUE_APPLIES_TO_ITEM, "01234561234561111012390*9*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239000",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239000*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239001",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239001*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239002",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239002*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239003",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239003*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239004",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239004*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239005",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239005*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239006",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239006*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239007",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239007*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239008",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239008*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "0123456123456111101239009",   GS1_LINTER_COUPON_MISSING_DONT_MULTIPLY_FLAG, "*0123456123456111101239009*");

	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012390000");
	UNIT_TEST_PASS(gs1_lint_couponcode, "01234561234561111012390001");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390002",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*2*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390003",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*3*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390004",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*4*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390005",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*5*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390006",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*6*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390007",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*7*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390008",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*8*");
	UNIT_TEST_FAIL(gs1_lint_couponcode, "01234561234561111012390009",  GS1_LINTER_COUPON_INVALID_DONT_MULTIPLY_FLAG, "0123456123456111101239000*9*");

	UNIT_TEST_FAIL(gs1_lint_couponcode, "012345612345611110123900000", GS1_LINTER_COUPON_EXCESS_DATA, "01234561234561111012390000*0*");

	/* North American Coupon Application Guideline Using GS1 DataBar Expanded Symbols R2.0 */
	UNIT_TEST_PASS(gs1_lint_couponcode, "10614141011111275110111");
	UNIT_TEST_PASS(gs1_lint_couponcode, "106141410222223100110222111101231023456721104561045678991201");
	UNIT_TEST_PASS(gs1_lint_couponcode, "1061414165432131501101201211014092110256100126663101231");
	UNIT_TEST_PASS(gs1_lint_couponcode, "106141410012342501106501013085093101231");
	UNIT_TEST_PASS(gs1_lint_couponcode, "106141410012471011076011110850921108609310123191000");
	UNIT_TEST_PASS(gs1_lint_couponcode, "106141411234562891101201212085010048000214025610048000310123191000");
	UNIT_TEST_PASS(gs1_lint_couponcode, "1061414154321031501101201211014092110256100126663101231");
	UNIT_TEST_PASS(gs1_lint_couponcode, "106141416543213500110000310123196000");

}

#endif  /* UNIT_TESTS */
