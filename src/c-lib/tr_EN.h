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

/*
 * Only an English translation of the library is provided.
 *
 * To create an alternative translation, create a copy of this file named
 * tr_<LANG>.h, localise it, and then rebuild defining GS1_ENCODERS_ERR_LANG =
 * <LANG>
 *
 */

#ifndef TR_EN_H
#define TR_EN_H

#define TR_EN_AI_TABLE_BROKEN_PREFIXES_DIFFER_IN_LENGTH "AI table is broken: AIs beginning '%c%c' have different lengths"
#define TR_EN_AI_DATA_IS_EMPTY "AI (%.*s) data is empty"
#define TR_EN_AI_DATA_HAS_INCORRECT_LENGTH "AI (%.*s) data has incorrect length"
#define TR_EN_AI_LINTER_ERROR "AI (%.*s): %s"
#define TR_EN_AI_VALUE_IS_TOO_SHORT "AI (%.*s) value is too short"
#define TR_EN_AI_VALUE_IS_TOO_LONG "AI (%.*s) value is too long"
#define TR_EN_AI_CONTAINS_ILLEGAL_CARAT_CHARACTER "AI (%.*s) contains illegal ^ character"
#define TR_EN_AI_UNRECOGNISED "Unrecognised AI: %.*s"
#define TR_EN_TOO_MANY_AIS "Too many AIs"
#define TR_EN_AI_PARSE_FAILED "Failed to parse AI data"
#define TR_EN_MISSING_FNC1_IN_FIRST_POSITION "Missing FNC1 in first position"
#define TR_EN_AI_DATA_EMPTY "The AI data is empty"
#define TR_EN_NO_AI_FOR_PREFIX "No known AI is a prefix of: %.4s..."
#define TR_EN_AI_DATA_IS_TOO_LONG "AI (%.*s) data is too long"
#define TR_EN_INVALID_AI_PAIRS "It is invalid to pair AI (%.*s) with AI (%.*s)"
#define TR_EN_REQUIRED_AIS_NOT_SATISFIED "Required AIs for AI (%.*s) are not satisfied: %s"
#define TR_EN_INSTANCES_OF_AI_HAVE_DIFFERENT_VALUES "Multiple instances of AI (%.*s) have different values"
#define TR_EN_SERIAL_NOT_PRESENT "Serial component must be present for AI (%.*s) when used with AI (8030)"
#define TR_EN_FAILED_TO_REALLOC_FOR_KEY_QUALIFIERS "Failed to reallocate memory for key-qualifiers"
#define TR_EN_FAILED_TO_MALLOC_FOR_KEY_QUALIFIERS "Failed to allocate memory for key-qualifiers"
#define TR_EN_URI_CONTAINS_ILLEGAL_CHARACTERS "URI contains illegal characters"
#define TR_EN_URI_CONTAINS_ILLEGAL_SCHEME "Scheme must be http:// or HTTP:// or https:// or HTTPS://"
#define TR_EN_URI_MISSING_DOMAIN_AND_PATH_INFO "URI must contain a domain and path info"
#define TR_EN_NO_GS1_DL_KEYS_FOUND_IN_PATH_INFO "No GS1 DL keys found in path info"
#define TR_EN_AI_VALUE_PATH_ELEMENT_IS_EMPTY "AI (%.*s) value path element is empty"
#define TR_EN_DECODED_AI_FROM_DL_PATH_INFO_CONTAINS_ILLEGAL_NULL "Decoded AI (%.*s) from DL path info contains illegal null character"
#define TR_EN_UNKNOWN_AI_IN_QUERY_PARAMS "Unknown AI (%.*s) in query parameters"
#define TR_EN_AI_VALUE_QUERY_ELEMENT_IN_EMPTY "AI (%.*s) value query element is empty"
#define TR_EN_DECODED_AI_VALUE_FROM_QUERY_PARAMS_CONTAINS_ILLEGAL_NULL "Decoded AI (%.*s) value from DL query params contains illegal null character"
#define TR_EN_INVALID_KEY_QUALIFIER_SEQUENCE "The AIs in the path are not a valid key-qualifier sequence for the key"
#define TR_EN_DUPLICATE_AI "AI (%.*s) is duplicated"
#define TR_EN_AI_IS_NOT_VALID_DATA_ATTRIBUTE "AI (%.*s) is not a valid DL URI data attribute"
#define TR_EN_AI_SHOULD_BE_IN_PATH_INFO "AI (%s) from query params should be in the path info"
#define TR_EN_DL_URI_PARSE_FAILED "Failed to parse DL data"
#define TR_EN_CANNOT_CREATE_DL_URI_WITHOUT_PRIMARY_KEY_AI "Cannot create a DL URI without a primary key AI"
#define TR_EN_UNKNOWN_SYMBOLOGY "Unknown symbology"
#define TR_EN_UNKNOWN_VALIDATION "Unknown validation"
#define TR_EN_VALIDATION_CANNOT_BE_AMENDED "This validation cannont be amended"
#define TR_EN_DATA_TOO_LONG "Maximum data length is %d characters"
#define TR_EN_PRIMARY_DATA_MUST_BE_N_DIGITS_WITHOUT_CHECK_DIGIT "Primary data must be %d digits without check digit"
#define TR_EN_PRIMARY_DATA_MUST_BE_N_DIGITS "Primary data must be %d digits"
#define TR_EN_PRIMARY_DATA_MUST_BE_ALL_DIGITS "Primary data must be all digits"
#define TR_EN_PRIMARY_DATA_CHECK_DIGIT_IS_INCORRECT "Primary data check digit is incorrect"
#define TR_EN_PRIMARY_DATA_IS_TOO_LARGE "Primary data item value is too large"
#define TR_EN_MISSING_SYMBOLOGY_IDENTIFIER "Missing symbology identifier"
#define TR_EN_UNSUPPORTED_SYMBOLOGY_IDENTIFIER "Unsupported symbology identifier"
#define TR_EN_PRIMARY_SCAN_DATA_IS_TOO_SHORT "Primary scan data is too short"
#define TR_EN_PRIMARY_MESSAGE_IS_TOO_LONG "Primary message is too long"
#define TR_EN_PRIMARY_MESSAGE_MAY_ONLY_CONTAIN_DIGITS "Primary message may only contain digits"
#define TR_EN_PRIMARY_MESSAGE_CHECK_DIGIT_IS_INCORRECT "Primary message check digit is incorrect"
#define TR_EN_SCAN_DATA_CONTAINS_ILLEGAL_CARAT "Scan data contains illegal ^ character"
#define TR_EN_FAILED_TO_PROCESS_SCAN_DATA "Failed to process scan data"
#define TR_EN_FORMAT_SPEC_FOR_OPT_COMPONENT_MISSING_RT_SQ_BRACKET "Format specification for optional component is missing ']': %s"
#define TR_EN_UNKNOWN_CHARACTER_SET "Unknown character set %c"
#define TR_EN_FORMAT_SPEC_TOO_SHORT "Format specification for component is too short: %s"
#define TR_EN_AI_LENGTH_TOO_LONG "AI length too long: %s"
#define TR_EN_AI_LENGTH_IS_NOT_A_NUMBER "AI length is not a number: %s"
#define TR_EN_UNRECOGNISED_FORMAT_SPECIFICATION "Unrecognised format specification for component: %s"
#define TR_EN_NUMBER_OF_LINTERS_EXCEEDS_IMPL_LIMIT "Number of linters for component exceeds implementation: %s"
#define TR_EN_UNKNOWN_LINTER "Unknown linter '%s'"
#define TR_EN_ENTRY_TOO_LONG "Entry too long"
#define TR_EN_SYNTAX_DICTIONARY_CAPACITY_TOO_SMALL "Syntax Dictionary capacity is too small"
#define TR_EN_AI_RANGE_HAS_WRONG_WIDTH "AI range has wrong width"
#define TR_EN_AIS_IN_RANGE_MUST_HAVE_EQUAL_WIDTH "AIs in range must have equal width"
#define TR_EN_AIS_MUST_BE_NUMERIC "AIs must be numeric"
#define TR_EN_AI_RANGE_PARTS_MAY_ONLY_DIFFER_IN_LAST_DIGIT "AI range parts may only differ in their last digit"
#define TR_EN_AI_RANGE_END_MUST_EXCEED_RANGE_START "AI range end must exceed range start"
#define TR_EN_AI_HAS_WRONG_WIDTH "AI has wrong width"
#define TR_EN_AI_MUST_BE_NUMBERIC "AI must be numeric"
#define TR_EN_TRUNCATED_AFTER_AI "Truncated after AI"
#define TR_EN_TRUNCATED_AFTER_FLAGS "Truncated after flags"
#define TR_EN_NUMBER_OF_AI_COMPONENTS_EXCEEDS_IMPL "Number of AI components exceeds implementation"
#define TR_EN_AI_IS_MISSING_COMPONENTS "AI is missing components"
#define TR_EN_ONLY_FINAL_COMPONENT_MAY_HAVE_VARIABLE_LENGTH "Only the final compoment may have variable length"
#define TR_EN_MANDATORY_COMPONENT_CANNOT_FOLLOW_OPTIONAL_COMPONENTS "A madatory component cannot follow optional components"
#define TR_EN_ATTRIBUTE_NAME_REQUIRED_ON_LHS_OF_ASSIGNMENT "Attribute name required on LHS of assignment"
#define TR_EN_ATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS "Attribute name contains illegal characters"
#define TR_EN_ATTRIBUTE_VALUE_CONTAINS_ILLEGAL_CHARACTERS "Attribute value contain illegal characters"
#define TR_EN_ATTRIBUTE_VALUE_REQUIRED_ON_RHS_OF_ASSIGNMENT "Attribute value required on RHS of assignment"
#define TR_EN_SINGLETON_ATTRIBUTE_NAME_CONTAINS_ILLEGAL_CHARACTERS "Singleton attribute name contains illegal characters"
#define TR_EN_ATTRIBUTES_TOO_LONG "Attributes too long"
#define TR_EN_FAILED_TO_ALLOCATE_MEMORY_FOR_ATTRS "Failed to allocate memory for attrs"
#define TR_EN_TITLE_CONTAINS_ILLEGAL_CHARACTERS "Title contain illegal characters"
#define TR_EN_FAILED_TO_ALLOCATE_MEMORY_FOR_TITLE "Failed to allocate memory for title"
#define TR_EN_FAILED_TO_ALLOCATE_AI_TABLE "Failed to allocate AI table"
#define TR_EN_CANNOT_READ_FILE "Cannot read file %s"
#define TR_EN_SYNTAX_DICTIONARY_LINE_EXCEEDS_IMPL "Syntax Dictionary line %d: Exceeds implementation limit of %d characters"
#define TR_EN_SYNTAX_DICTIONARY_LINE_ERROR "Syntax Dictionary line %d: %.*s"

#endif  /* TR_EN_H */
