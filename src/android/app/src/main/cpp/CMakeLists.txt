cmake_minimum_required (VERSION 3.18.1)
project (gs1encoders)

set(version "1.0")

SET(LIB_SOURCE_FILES
gs1encoders/ai.c
gs1encoders/dl.c
gs1encoders/scandata.c
gs1encoders/syn.c
gs1encoders/gs1encoders.c
gs1encoders/syntax/gs1syntaxdictionary.c
gs1encoders/syntax/lint_couponcode.c
gs1encoders/syntax/lint_couponposoffer.c
gs1encoders/syntax/lint_cset39.c
gs1encoders/syntax/lint_cset64.c
gs1encoders/syntax/lint_cset82.c
gs1encoders/syntax/lint_csetnumeric.c
gs1encoders/syntax/lint_csumalpha.c
gs1encoders/syntax/lint_csum.c
gs1encoders/syntax/lint_hhmm.c
gs1encoders/syntax/lint_hyphen.c
gs1encoders/syntax/lint_iban.c
gs1encoders/syntax/lint_importeridx.c
gs1encoders/syntax/lint_iso3166999.c
gs1encoders/syntax/lint_iso3166alpha2.c
gs1encoders/syntax/lint_iso3166.c
gs1encoders/syntax/lint_iso3166list.c
gs1encoders/syntax/lint_iso4217.c
gs1encoders/syntax/lint_key.c
gs1encoders/syntax/lint_latitude.c
gs1encoders/syntax/lint_longitude.c
gs1encoders/syntax/lint_mediatype.c
gs1encoders/syntax/lint_mmoptss.c
gs1encoders/syntax/lint_nonzero.c
gs1encoders/syntax/lint_nozeroprefix.c
gs1encoders/syntax/lint_pcenc.c
gs1encoders/syntax/lint_pieceoftotal.c
gs1encoders/syntax/lint_winding.c
gs1encoders/syntax/lint_yesno.c
gs1encoders/syntax/lint_yymmd0.c
gs1encoders/syntax/lint_yymmdd.c
gs1encoders/syntax/lint_yymmddhh.c
gs1encoders/syntax/lint_zero.c
)

add_compile_definitions(GS1_LINTER_ERR_STR_EN)

add_library(gs1encoders SHARED ${LIB_SOURCE_FILES} native-lib.c)
