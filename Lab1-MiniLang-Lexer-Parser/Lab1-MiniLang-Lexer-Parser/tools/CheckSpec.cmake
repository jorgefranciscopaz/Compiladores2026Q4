# Checks spec/MiniLang.l (Lab1.md, steps A1 and A2). Provided: do not modify.
#
#   cmake -DREFLEX=<reflex> -DSPEC=<spec/MiniLang.l> -DWORKDIR=<scratch dir> -P tools/CheckSpec.cmake
#
# It checks that
#   1. the TODO(A1) marker has been replaced by rules;
#   2. the spec contains no keyword rule (keywords are found by table lookup, not by the DFA);
#   3. RE/flex accepts the spec and can write its DFA graph.
# The generated files go to WORKDIR (a build directory), never to your source tree.
cmake_minimum_required(VERSION 3.20)

foreach(var REFLEX SPEC WORKDIR)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "usage: cmake -DREFLEX=... -DSPEC=... -DWORKDIR=... -P tools/CheckSpec.cmake")
  endif()
endforeach()

file(READ "${SPEC}" spec)

if(spec MATCHES "TODO\\(A1\\)")
  message(FATAL_ERROR "${SPEC} still has the TODO(A1) marker: write the rules (Lab1.md, step A1)")
endif()

if(spec MATCHES "\"(func|int|bool|void|if|else|while|for|return|print|read|true|false)\"")
  message(FATAL_ERROR "${SPEC} has a rule for a keyword (\"${CMAKE_MATCH_1}\"). Keywords are NOT part of the DFA: the identifier rule matches them and Lexer.cpp tells them apart with a table lookup (Lab1.md, step A5).")
endif()

file(MAKE_DIRECTORY "${WORKDIR}")
execute_process(
  COMMAND "${REFLEX}" "--graphs-file=${WORKDIR}/spec.gv" "--outfile=${WORKDIR}/lex.yy.cpp" "${SPEC}"
  RESULT_VARIABLE rc
  OUTPUT_VARIABLE out
  ERROR_VARIABLE err
)
if(NOT "${rc}" STREQUAL "0")
  message(FATAL_ERROR "reflex rejected ${SPEC} (exit ${rc}):\n${out}${err}")
endif()
if(NOT EXISTS "${WORKDIR}/spec.gv")
  message(FATAL_ERROR "reflex did not write the DFA graph")
endif()
message(STATUS "spec OK: reflex accepted ${SPEC}")
