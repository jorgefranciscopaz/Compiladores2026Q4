# Gives readable names to the states of a RE/flex DFA graph. Provided: do not modify.
#
#   cmake -DIN=docs/MiniLang.gv -DOUT=docs/MiniLang.numbered.gv -P tools/NumberDfa.cmake
#
# `reflex --graphs-file` names every state after a memory address (N0x7f3a...), which is
# useless for talking about the automaton. This script renames the states S0, S1, S2, ... in
# the ORDER IN WHICH THEY APPEAR in the .gv file (S0 is the start state, the one marked
# /*START*/) and writes the label "Sk" inside each node. Accepting states (double circle)
# also show the number of the RE/flex rule they accept, e.g.  S7 [4].
#
# The input file is NOT modified. Render the result with:
#   dot -Tsvg docs/MiniLang.numbered.gv -o docs/MiniLang.numbered.svg
#
# The numbering depends only on the order of the nodes in the .gv, which RE/flex generates
# deterministically from your spec: if you edit spec/MiniLang.l and regenerate the graph,
# the numbers can change, and then your Lexer.cpp and docs/TRACEABILITY.md must be updated.
cmake_minimum_required(VERSION 3.20)

if("${IN}" STREQUAL "" OR "${OUT}" STREQUAL "")
  message(FATAL_ERROR "usage: cmake -DIN=<reflex .gv> -DOUT=<numbered .gv> -P tools/NumberDfa.cmake")
endif()
if(NOT EXISTS "${IN}")
  message(FATAL_ERROR "NumberDfa: '${IN}' does not exist. Run reflex with --graphs-file first (Lab1.md, step A2).")
endif()

file(READ "${IN}" content)

# Every state is declared in the file by a line such as
#   /*START*/<TAB>N0x7f3a1c0e0018 [label=""];
#   /*STATE*/<TAB>N0x7f3a1c0e0888 [label=""];
#   /*ACCEPT 6*/<TAB>N0x7f3a1c0e01c8 [label="[6]",peripheries=2];
string(REGEX MATCHALL "/\\*(START|STATE|ACCEPT [0-9]+)\\*/[ \t]+N0x[0-9a-fA-F]+" declarations "${content}")
list(LENGTH declarations count)
if(count EQUAL 0)
  message(FATAL_ERROR "NumberDfa: no states found in '${IN}'. Is it a graph written by reflex --graphs-file?")
endif()

set(k 0)
foreach(declaration IN LISTS declarations)
  string(REGEX MATCH "N0x[0-9a-fA-F]+" id "${declaration}")
  string(REGEX MATCH "ACCEPT ([0-9]+)" accept "${declaration}")
  set(rule "${CMAKE_MATCH_1}")

  # the id is followed by a space, ';' or end of line: never by another hex digit
  string(REGEX REPLACE "${id}([^0-9a-fA-F])" "S${k}\\1" content "${content}")

  if("${rule}" STREQUAL "")
    string(REPLACE "S${k} \[label=\"\"" "S${k} \[label=\"S${k}\"" content "${content}")
    if(declaration MATCHES "START")
      message(STATUS "S${k}\tstart state")
    else()
      message(STATUS "S${k}\tnot accepting")
    endif()
  else()
    string(REPLACE "S${k} \[label=\"\[${rule}\]\"" "S${k} \[label=\"S${k}\\n\[${rule}\]\"" content "${content}")
    message(STATUS "S${k}\taccepting, RE/flex rule ${rule}")
  endif()
  math(EXPR k "${k} + 1")
endforeach()

file(WRITE "${OUT}" "${content}")
message(STATUS "NumberDfa: ${count} states, written to ${OUT} (S0 = start state)")
