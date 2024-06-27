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
 * @file lint_iso3166list.c
 *
 * @brief Not used.
 *
 */


#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Not used.
 *
 * @deprecated Function retained for API compatibility purposes only. Not
 * longer referenced by the Syntax Dictionary.
 *
 * @param [in] data Not used.
 * @param [out] err_pos Not used.
 * @param [out] err_len Not used.
 *
 * @return #GS1_LINTER_OK
 *
 */
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t gs1_lint_iso3166list(const char* const data, size_t* const err_pos, size_t* const err_len)
{
	(void)data;
	(void)err_pos;
	(void)err_len;
	return GS1_LINTER_OK;
}
