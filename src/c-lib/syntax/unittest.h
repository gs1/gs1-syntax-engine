/**
 * GS1 Syntax Dictionary
 *
 * @author Copyright (c) 2022 GS1 AISBL.
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


#ifndef LINT_UNIT_TEST
#define LINT_UNIT_TEST

#include "gs1syntaxdictionary.h"

#define TEST_NO_MAIN
#include "acutest.h"

#define UNIT_TEST_PASS(f, g) DO_UNIT_TEST(1, f, g, 0, NULL, __FILE__, __LINE__)
#define UNIT_TEST_FAIL(f, g, e, h) DO_UNIT_TEST(0, f, g, e, h, __FILE__, __LINE__)

static void DO_UNIT_TEST(int should_succeed, gs1_lint_err_t (*fn)(const char *, size_t *, size_t *), const char *data, gs1_lint_err_t expect_err, const char *expect_highlight, const char *file, int line) {

	gs1_lint_err_t err;
	size_t err_pos[1], err_len[1];
	char highlight[100] = {0};
	char casestr[512] = {0};

	strcat(casestr, *data != '\0' ? data : "(empty string)");
	sprintf(&casestr[strlen(casestr)], " %s:%d", file, line);
	TEST_CASE(casestr);

	err = (fn)(data, err_pos, err_len);
	TEST_ASSERT(err < __GS1_LINTER_NUM_ERRS);

	if (should_succeed) {
		TEST_CHECK(err == GS1_LINTER_OK);
		TEST_MSG("Expected success, but failed with error: %s", gs1_lint_err_str[err]);
		return;
	}

	TEST_CHECK(err == expect_err);
	TEST_MSG("Got: %s; Expected: %s", gs1_lint_err_str[err], gs1_lint_err_str[expect_err]);

	if (err == GS1_LINTER_OK)
		return;

	TEST_ASSERT(*err_pos < strlen(data) || *err_pos == 0);
	TEST_ASSERT(*err_pos + *err_len <= strlen(data));

	strncat(highlight, data, *err_pos);
	strcat(highlight, "*");
	strncat(highlight, &data[*err_pos], *err_len);
	strcat(highlight, "*");
	strncat(highlight, &data[*err_pos + *err_len], strlen(data) - *err_pos - *err_len);

	TEST_CHECK(strcmp(highlight, expect_highlight) == 0);
	TEST_MSG("Got: %s; Expected: %s", highlight, expect_highlight);

}

#endif  /* LINT_UNIT_TEST */
