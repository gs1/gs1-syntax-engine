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

#include <stddef.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * @brief Generates a stub function to replace legacy linters.
 *
 * The generated stub function is retained for API compatibility purposes only
 * and is no longer referenced by the Syntax Dictionary.
 *
 */
#define GS1_LINTER_STUB(linter)					\
GS1_SYNTAX_DICTIONARY_API DEPRECATED gs1_lint_err_t linter(	\
				const char* const data,		\
				size_t* const err_pos,		\
				size_t* const err_len)		\
{								\
	(void)data;						\
	(void)err_pos;						\
	(void)err_len;						\
	GS1_LINTER_RETURN_OK;					\
}


GS1_LINTER_STUB(gs1_lint_hhmm)
GS1_LINTER_STUB(gs1_lint_iso3166list)
GS1_LINTER_STUB(gs1_lint_mmoptss)
GS1_LINTER_STUB(gs1_lint_yymmddhh)


#undef GS1_LINTER_STUB

