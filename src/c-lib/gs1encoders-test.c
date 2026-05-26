/**
 * GS1 Barcode Syntax Engine
 *
 * @author Copyright (c) 2021-2026 GS1 AISBL.
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
 *  Permit sprintf on MacOS clang.
 *  Don't perform code analysis of third-party code that is only used for testing.
 *
 */
#if defined(__clang__)
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=pure"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#elif defined(_MSC_VER)
#  include <CodeAnalysis/warnings.h>
#  pragma warning(push)
#  pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#endif
#include "acutest.h"
#if defined(__clang__)
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#include <stddef.h>

#include "enc-private.h"
#include "dl.h"
#include "scandata.h"
#include "syn.h"


TEST_LIST = {

    /*
     * gs1encoders.c
     *
     */
    { "api_getVersion", test_api_getVersion },
    { "api_instanceSize", test_api_instanceSize },
    { "api_init", test_api_init },
    { "api_init_deprecatedFlags", test_api_init_deprecatedFlags },
    { "api_init_opts_layout", test_api_init_opts_layout },
    { "api_init_enum_values", test_api_init_enum_values },
    { "api_defaults", test_api_defaults },
    { "api_sym", test_api_sym },
    { "api_addCheckDigit", test_api_addCheckDigit },
    { "api_permitUnknownAIs", test_api_permitUnknownAIs },
    { "api_permitZeroSuppressedGTINinDLuris", test_api_permitZeroSuppressedGTINinDLuris },
    { "api_validateAIassociations", test_api_validateAIassociations },
    { "api_validations", test_api_validations },
    { "api_getters", test_api_getters },
    { "api_dataStr", test_api_dataStr },
    { "api_getAIdataStr", test_api_getAIdataStr },
    { "api_getScanData", test_api_getScanData },
    { "api_setScanData", test_api_setScanData },
    { "api_getHRI", test_api_getHRI },
    { "api_copyHRI", test_api_copyHRI },
    { "api_getDLignoredQueryParams", test_api_getDLignoredQueryParams },
    { "api_copyDLignoredQueryParams", test_api_copyDLignoredQueryParams },
    { "api_allocFailures", test_api_allocFailures },
#ifndef EXCLUDE_SYNTAX_DICTIONARY_LOADER
    { "api_brokenPrefixSyndict", test_api_brokenPrefixSyndict },
#endif


    /*
     * syn.c
     *
     */
#ifndef EXCLUDE_SYNTAX_DICTIONARY_LOADER
    { "syn_parseSyntaxDictionaryEntry", test_syn_parseSyntaxDictionaryEntry },
    { "syn_attrTemplateForm", test_syn_attrTemplateForm },
    { "syn_allocFailures", test_syn_allocFailures },
    { "syn_capacityOverflow", test_syn_capacityOverflow },
    { "syn_strdupFailures", test_syn_strdupFailures },
    { "syn_lineTooLong", test_syn_lineTooLong },
    { "syn_nulLeadingLine", test_syn_nulLeadingLine },
    { "syn_dictionaryOrder", test_syn_dictionaryOrder },
#endif


    /*
     * ai.c
     *
     */
    { "ai_lookupAIentry", test_ai_lookupAIentry },
    { "ai_existsInAIdata", test_ai_existsInAIdata },
    { "ai_test_ai_checkAIlengthByPrefix", test_ai_checkAIlengthByPrefix },
    { "ai_AItableVsPrefixLength", test_ai_AItableVsPrefixLength },
    { "ai_AItableVsIsFNC1required", test_ai_AItableVsIsFNC1required },
    { "ai_gs1_parseAIdata", test_ai_parseAIdata },
    { "ai_linters", test_ai_linters },
    { "ai_errMarkup", test_ai_errMarkup },
    { "ai_gs1_processAIdata", test_ai_processAIdata },
    { "ai_validateAIs", test_ai_validateAIs },


    /*
     * dl.c
     *
     */
    { "dl_testValidateDLpathAIseq", test_dl_testValidateDLpathAIseq },
    { "dl_gs1_parseDLuri", test_dl_parseDLuri },
    { "dl_URIunescape", test_dl_URIunescape },
    { "dl_URIescape", test_dl_URIescape },
    { "dl_generateDLuri", test_dl_generateDLuri },
    { "dl_allocFailures", test_dl_allocFailures },
    { "dl_keyQualifierLimit", test_dl_keyQualifierLimit },


    /*
     * scandata.c
     *
     */
    { "scandata_validateParity", test_scandata_validateParity },
    { "scandata_generateScanData", test_scandata_generateScanData },
    { "scandata_processScanData", test_scandata_processScanData },

    { NULL, NULL }
};
