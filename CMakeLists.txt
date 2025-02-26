cmake_minimum_required(VERSION 3.25)
project(CplCompiler LANGUAGES C)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(
    Parser
    "src/parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/parser.c"
    DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/parser.h"
)

flex_target(Lexer "src/lexer.lex" "${CMAKE_CURRENT_BINARY_DIR}/lexer.c")

add_flex_bison_dependency(Lexer Parser)

configure_file(
    "src/bison_types.h"
    "${CMAKE_CURRENT_BINARY_DIR}/bison_types.h"
    COPYONLY
)

add_executable(
    cpq
    "src/bison_types.h"
    "src/cpq.c"
    "src/list.c"
    "src/list.h"
    "src/utils.c"
    "src/utils.h"
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_include_directories(cpq PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
