# Checks the DFA <-> code traceability deliverables (Lab1.md, step A6). Provided: do not modify.
#
#   cmake -DLAB_DIR=<lab folder> -P tools/CheckTraceability.cmake        (or: ctest -R docs.traceability)
#
# What is checked (it proves the three artifacts talk about the SAME automaton; it cannot
# prove that your explanations are good: that is what you are practicing):
#   1. docs/MiniLang.gv, docs/MiniLang.svg and docs/MiniLang.numbered.svg exist;
#   2. for EVERY state S0..S(n-1) of docs/MiniLang.gv (n = number of nodes written by reflex),
#      src/Lexer.cpp has `case k:` and a comment mentioning `Sk` ON THE SAME LINE;
#   3. docs/TRACEABILITY.md has a table row starting with `| Sk |` for every state;
#   4. docs/TRACEABILITY.md contains no `<TODO>` placeholder.
cmake_minimum_required(VERSION 3.20)

if("${LAB_DIR}" STREQUAL "")
  message(FATAL_ERROR "usage: cmake -DLAB_DIR=<lab folder> -P tools/CheckTraceability.cmake")
endif()

set(GV       "${LAB_DIR}/docs/MiniLang.gv")
set(SVG      "${LAB_DIR}/docs/MiniLang.svg")
set(NUMSVG   "${LAB_DIR}/docs/MiniLang.numbered.svg")
set(LEXER    "${LAB_DIR}/src/Lexer.cpp")
set(TRACE    "${LAB_DIR}/docs/TRACEABILITY.md")

set(problems "")
macro(problem text)
  string(APPEND problems "  - ${text}\n")
endmacro()

foreach(f "${GV}" "${SVG}" "${NUMSVG}" "${LEXER}" "${TRACE}")
  if(NOT EXISTS "${f}")
    problem("missing file: ${f}")
  endif()
endforeach()

if(NOT EXISTS "${GV}" OR NOT EXISTS "${LEXER}" OR NOT EXISTS "${TRACE}")
  message(FATAL_ERROR "traceability check cannot continue:\n${problems}(see Lab1.md, steps A2 to A6)")
endif()

file(READ "${GV}" gv)
string(REGEX MATCHALL "/\\*(START|STATE|ACCEPT [0-9]+)\\*/" states "${gv}")
list(LENGTH states n)
if(n EQUAL 0)
  message(FATAL_ERROR "'${GV}' has no states: is it the file written by `reflex --graphs-file`?")
endif()

file(READ "${LEXER}" lexer)
file(READ "${TRACE}" trace)

math(EXPR last "${n} - 1")
foreach(k RANGE 0 ${last})
  string(REGEX MATCH "case[ \t]+${k}[ \t]*:[^\n]*S${k}([^0-9]|$)" in_code "${lexer}")
  if("${in_code}" STREQUAL "")
    problem("src/Lexer.cpp: no line with both `case ${k}:` and a comment mentioning S${k}")
  endif()
  string(REGEX MATCH "(^|\n)\\|[ \t]*S${k}[ \t]*\\|" in_table "${trace}")
  if("${in_table}" STREQUAL "")
    problem("docs/TRACEABILITY.md: no table row starting with `| S${k} |`")
  endif()
endforeach()

if(trace MATCHES "<TODO>")
  problem("docs/TRACEABILITY.md still contains <TODO> placeholders")
endif()

if(NOT "${problems}" STREQUAL "")
  message(FATAL_ERROR "traceability problems (the automaton in docs/MiniLang.gv has ${n} states, S0..S${last}):\n${problems}")
endif()
message(STATUS "traceability OK: ${n} states (S0..S${last}) found in the graph, in Lexer.cpp and in TRACEABILITY.md")
