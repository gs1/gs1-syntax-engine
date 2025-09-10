/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_couponposoffer.c
 *
 * @brief The `couponposoffer` linter ensures that the data conforms to the
 * modernised North American positive offer file coupon code standard, as
 * carried in AI (8112).
 *
 * @remark Refer to the document "GS1 AI (8112) Coupon Data Specification" for
 * a description of the positive offer file coupon code data contents.
 *
 */


#include <assert.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to the modernised North
 * American positive offer file coupon code standard, as carried in AI (8112).
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_COUPON_MISSING_FORMAT_CODE if the data is missing a
 *         Format Code.
 * @return #GS1_LINTER_COUPON_INVALID_FORMAT_CODE if the data contains an
 *         invalid Format Code, neither "0" or "1".
 * @return #GS1_LINTER_COUPON_MISSING_FUNDER_VLI if the data is missing a
 *         Funder ID length indicator.
 * @return #GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH if the data contains a
 *         Funder ID length indicator that is not within the range "0" to "6".
 * @return #GS1_LINTER_COUPON_TRUNCATED_FUNDER if the data contains a Funder
 *         ID is that is shorter than specified by its length indicator.
 * @return #GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE if the data contains an
 *         Offer Code that is shorter than six digits.
 * @return #GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI if the data is missing
 *         a Serial Number length indicator.
 * @return #GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER if the data contains a
 *         Serial Number that is shorter than specified by its length indicator.
 * @return #GS1_LINTER_COUPON_EXCESS_DATA if the data contains excess data
 *         following the Serial Number.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_couponposoffer(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;
	int vli;
	const char *p, *q;

	assert(data);


	/*
	 * Data must consist of all digits.
	 *
	 */
	for (pos = 0; pos < data_len; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);
	}

	p = data;
	q = data + data_len;


	/*
	 * Validate the Format ID.
	 *
	 * Valid Format IDs are "0" and "1".
	 *
	 */
	if (GS1_LINTER_UNLIKELY(p == q))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_MISSING_FORMAT_CODE,
			0,
			(size_t)(q - data)
		);

	if (GS1_LINTER_UNLIKELY(*p != '0' && *p != '1'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_INVALID_FORMAT_CODE,
			(size_t)(p - data),
			1
		);


	/*
	 * Validate the Funder VLI is in the range "0" to "6" and that the
	 * following Funder ID has the corresponding length (plus 6).
	 *
	 */
	if (GS1_LINTER_UNLIKELY(++p == q))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_MISSING_FUNDER_VLI,
			0,
			(size_t)(q - data)
		);
	if (GS1_LINTER_UNLIKELY(*p > '6'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH,
			(size_t)(p - data),
			1
		);
	vli = *p - '0' + 6;

	if (GS1_LINTER_UNLIKELY(q - ++p < vli))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_TRUNCATED_FUNDER,
			(p == q) ? 0                  : (size_t)(p - data),
			(p == q) ? (size_t)(q - data) : (size_t)(q - p)
		);

	p += vli;


	/*
	 * Validate the existence of the six digit Offer Code.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(q - p < 6))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE,
			(p == q) ? 0                  : (size_t)(p - data),
			(p == q) ? (size_t)(q - data) : (size_t)(q - p)
		);

	p += 6;


	/*
	 * Validate that the Serial Number follows its VLI and has the
	 * corresponding length (plus 6).
	 *
	 */
	if (GS1_LINTER_UNLIKELY(p == q))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI,
			0,
			(size_t)(q - data)
		);
	vli = *p - '0' + 6;

	if (GS1_LINTER_UNLIKELY(q - ++p < vli))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER,
			(p == q) ? 0                  : (size_t)(p - data),
			(p == q) ? (size_t)(q - data) : (size_t)(q - p)
		);

	p += vli;


	/*
	 * Report any excess data that follows the Serial Number.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(p != q))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_COUPON_EXCESS_DATA,
			(size_t)(p - data),
			(size_t)(q - p)
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_couponposoffer(void)
{

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "a01234561234560123456", GS1_LINTER_NON_DIGIT_CHARACTER, "*a*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456012345a", GS1_LINTER_NON_DIGIT_CHARACTER, "00123456123456012345*a*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "", GS1_LINTER_COUPON_MISSING_FORMAT_CODE, "**");

	UNIT_TEST_PASS(gs1_lint_couponposoffer, "001234561234560123456");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "101234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "201234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*2*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "301234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*3*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "401234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*4*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "501234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*5*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "601234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*6*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "701234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*7*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "801234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*8*01234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "901234561234560123456", GS1_LINTER_COUPON_INVALID_FORMAT_CODE, "*9*01234561234560123456");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0",        GS1_LINTER_COUPON_MISSING_FUNDER_VLI, "*0*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00",       GS1_LINTER_COUPON_TRUNCATED_FUNDER, "*00*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001",      GS1_LINTER_COUPON_TRUNCATED_FUNDER, "00*1*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012",     GS1_LINTER_COUPON_TRUNCATED_FUNDER, "00*12*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123",    GS1_LINTER_COUPON_TRUNCATED_FUNDER, "00*123*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234",   GS1_LINTER_COUPON_TRUNCATED_FUNDER, "00*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345",  GS1_LINTER_COUPON_TRUNCATED_FUNDER, "00*12345*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456",       GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*00123456*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "01123456",       GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "01*123456*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "011234567",      GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*011234567*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "021234567",      GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "02*1234567*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0212345678",     GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*0212345678*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0312345678",     GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "03*12345678*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "03123456789",    GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*03123456789*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "04123456789",    GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "04*123456789*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "041234567890",   GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*041234567890*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "051234567890",   GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "05*1234567890*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0512345678901",  GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*0512345678901*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0612345678901",  GS1_LINTER_COUPON_TRUNCATED_FUNDER,     "06*12345678901*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "06123456789012", GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "*06123456789012*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "07", GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH, "0*7*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "08", GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH, "0*8*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "09", GS1_LINTER_COUPON_INVALID_FUNDER_LENGTH, "0*9*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561",     GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "00123456*1*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612",    GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "00123456*12*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123",   GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "00123456*123*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234",  GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "00123456*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345", GS1_LINTER_COUPON_TRUNCATED_OFFER_CODE, "00123456*12345*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456", GS1_LINTER_COUPON_MISSING_SERIAL_NUMBER_VLI, "*00123456123456*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234560",               GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "*001234561234560*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345601",              GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234560*1*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456012",             GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234560*12*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234560123",            GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234560*123*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345601234",           GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234560*1234*");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456012345",          GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234560*12345*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "001234561234560123456");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234561123456",         GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234561*123456*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "0012345612345611234567");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345621234567",        GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234562*1234567*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "00123456123456212345678");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456312345678",       GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234563*12345678*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "001234561234563123456789");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234564123456789",      GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234564*123456789*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "0012345612345641234567890");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345651234567890",     GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234565*1234567890*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "00123456123456512345678901");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456612345678901",    GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234566*12345678901*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "001234561234566123456789012");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "001234561234567123456789012",   GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234567*123456789012*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "0012345612345671234567890123");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345681234567890123",  GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234568*1234567890123*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "00123456123456812345678901234");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "00123456123456912345678901234", GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "001234561234569*12345678901234*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "001234561234569123456789012345");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0012345612345601234567",                GS1_LINTER_COUPON_EXCESS_DATA, "001234561234560123456*7*");

	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "06123456789012123456912345678901234",   GS1_LINTER_COUPON_TRUNCATED_SERIAL_NUMBER, "061234567890121234569*12345678901234*");
	UNIT_TEST_PASS(gs1_lint_couponposoffer, "061234567890121234569123456789012345");
	UNIT_TEST_FAIL(gs1_lint_couponposoffer, "0612345678901212345691234567890123456", GS1_LINTER_COUPON_EXCESS_DATA, "061234567890121234569123456789012345*6*");

}

#endif  /* UNIT_TESTS */
