# Counts the tests the student has written under tests/student. Provided: do not modify.
#
#   cmake -DDIR=<tests/student> -DMIN_LEXER=n -DMIN_VALID=n -DMIN_INVALID=n -P tools/CheckStudentTests.cmake
#
# A lexer test is  <name>.txt + <name>.tokens   in  lexer/
# A valid test is  <name>.txt                   in  parser/valid/
# An invalid test is <name>.txt + <name>.expected in parser/invalid/
cmake_minimum_required(VERSION 3.20)

function(count_tests subdir ext out_var)
  file(GLOB inputs "${DIR}/${subdir}/*.txt")
  set(n 0)
  foreach(input IN LISTS inputs)
    get_filename_component(name "${input}" NAME_WE)
    if("${ext}" STREQUAL "" OR EXISTS "${DIR}/${subdir}/${name}.${ext}")
      math(EXPR n "${n} + 1")
    endif()
  endforeach()
  set(${out_var} ${n} PARENT_SCOPE)
endfunction()

count_tests(lexer tokens lexer_n)
count_tests(parser/valid "" valid_n)
count_tests(parser/invalid expected invalid_n)

set(problems "")
if(lexer_n LESS MIN_LEXER)
  string(APPEND problems "  - lexer tests: ${lexer_n} found in tests/student/lexer, at least ${MIN_LEXER} required (each needs <name>.txt and <name>.tokens)\n")
endif()
if(valid_n LESS MIN_VALID)
  string(APPEND problems "  - valid parser tests: ${valid_n} found in tests/student/parser/valid, at least ${MIN_VALID} required\n")
endif()
if(invalid_n LESS MIN_INVALID)
  string(APPEND problems "  - invalid parser tests: ${invalid_n} found in tests/student/parser/invalid, at least ${MIN_INVALID} required (each needs <name>.txt and <name>.expected)\n")
endif()

if(NOT "${problems}" STREQUAL "")
  message(FATAL_ERROR "not enough tests of your own yet:\n${problems}")
endif()
message(STATUS "own tests: ${lexer_n} lexer, ${valid_n} valid, ${invalid_n} invalid")
