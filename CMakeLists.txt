cmake_minimum_required(VERSION 3.25)
project(QuadCompiler)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(Parser "parser.y" "${CMAKE_CURRENT_BINARY_DIR}/parser.c")
flex_target(Lexer "lexer.l" "${CMAKE_CURRENT_BINARY_DIR}/lexer.c")
add_flex_bison_dependency(Lexer Parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(qcc "main.cxx" ${FLEX_Lexer_OUTPUTS})
