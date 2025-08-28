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
 * @file gs1syntaxdictionary-utils.h
 *
 * @brief This header provides utility macros used by the reference linter
 * functions.
 *
 */

#ifndef GS1_SYNTAXDICTIONARY_UTILS_H
#define GS1_SYNTAXDICTIONARY_UTILS_H

#include "gs1syntaxdictionary.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


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


/**
 * @def GS1_LINTER_LIKELY
 * @brief Implemention may provide hint to the compiler that the expression is
 * likely to be true.
 *
 */
#if defined(__GNUC__) || defined(__clang__)
#  define GS1_LINTER_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#  define GS1_LINTER_LIKELY(x) (x)
#endif


/**
 * @def GS1_LINTER_UNLIKELY
 * @brief Implemention may provide hint to the compiler that the expression is
 * likely to be false.
 *
 */
#if defined(__GNUC__) || defined(__clang__)
#  define GS1_LINTER_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define GS1_LINTER_UNLIKELY(x) (x)
#endif


/**
 * @brief Return from a linter indicating that no problem was detected with the
 * given data.
 *
 * @note For many linters this does not guarantee that the data is valid, only
 * that it satisfies all of the checks performed by the linter.
 *
 */
#define GS1_LINTER_RETURN_OK 		\
do {					\
	return GS1_LINTER_OK;		\
} while (0)


/**
 * @brief Return from a linter indicating that a problem was detected with the
 * given data.
 *
 * @param [in] error An error of type ::gs1_lint_err_t to return.
 * @param [in] position The starting position of the invalid data.
 * @param [in] length The length of the invalid data segment.
 *
 */
#define GS1_LINTER_RETURN_ERROR(error, position, length)	\
do {								\
	if (err_pos) *err_pos = position;			\
	if (err_len) *err_len = length;				\
	return error;						\
} while (0)


/**
 * @brief Perform a lookup of a position in a bit field
 *
 * @param [in] bit The bit position in the field to lookup. The position
 *                 must be in the field otherwise the behaviour is undefined.
 * @param [in] field An array whose elements (of type with arbitrary size) when
 *                   concatinated produce a single bit field whose positions
 *                   are numbered from left to right (MSB of first element to
 *                   LSB of last element).
 * @param [out] valid Set to `1` if the position numbered `bit` is set in the
 *                    `field`, otherwise set to `0`.
 *
 */
#define GS1_LINTER_BITFIELD_LOOKUP(bit, field, valid)				\
do {										\
	int w = CHAR_BIT * sizeof(field[0]);					\
	assert((size_t)(bit/w) < sizeof(field) / sizeof(field[0]));		\
	valid = (field[bit/w] & (UINT64_C(1) << (w-1) >> (bit%w))) ? 1 : 0;	\
} while (0)


/**
 * @brief Perform a binary search for a search term in a sorted set of strings.
 *
 * @param [in] needle A search term string to be exactly matched.
 * @param [in] haystack A array of strings to be searched. The array must be
 *                      otherwise the matching behaviour is undefined.
 * @param [out] valid Set to `1` if `needle` is found in `haystack`, otherwise
 *                    set to `0`.
 *
 */
#define GS1_LINTER_BINARY_SEARCH(needle, haystack, valid)	\
do {								\
	size_t s = 0;						\
	size_t e = sizeof(haystack) / sizeof(haystack[0]);	\
	valid = 0;						\
	while (s < e) {						\
		const size_t m = s + (e - s) / 2;		\
		const int cmp = strcmp(haystack[m], needle);	\
		if (cmp < 0)					\
			s = m + 1;				\
		else if (cmp > 0)				\
			e = m;					\
		else {						\
			valid = 1;				\
			break;					\
		}						\
	}							\
} while (0)

#endif  /* GS1_SYNTAXDICTIONARY_UTILS_H */
