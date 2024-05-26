cmake_minimum_required (VERSION 3.22)
project (gs1encoders)

set(version "1.0")

file(
GLOB
LIB_SOURCE_FILES
gs1encoders/ai.c
gs1encoders/dl.c
gs1encoders/scandata.c
gs1encoders/syn.c
gs1encoders/gs1encoders.c
gs1encoders/syntax/gs1syntaxdictionary.c
gs1encoders/syntax/lint_*.c
)

add_compile_definitions(GS1_LINTER_ERR_STR_EN)

add_library(gs1encoders SHARED ${LIB_SOURCE_FILES} native-lib.c)
