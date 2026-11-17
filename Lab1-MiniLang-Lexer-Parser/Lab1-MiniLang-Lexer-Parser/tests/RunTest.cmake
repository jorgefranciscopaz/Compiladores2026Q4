# Runs ONE test of the lab. Provided: do not modify.
#
#   cmake -DEXE=<minilang> -DMODE=<tokens|valid|invalid> -DINPUT=<file> [-DEXPECTED=<file>] -P RunTest.cmake
#
#   MODE=tokens   `minilang --tokens INPUT` must exit 0 and print exactly the text of EXPECTED
#                 (EXPECTED = <name>.tokens, format: LINE:COL TokenId 'text').
#   MODE=valid    `minilang INPUT` must accept the program (exit 0).
#   MODE=invalid  `minilang INPUT` must reject the program with a syntax error (exit 2) reported
#                 at the position written in EXPECTED (EXPECTED = <name>.expected, one line:
#                 LINE:COL). The text of the message is free.
cmake_minimum_required(VERSION 3.20)

foreach(var EXE MODE INPUT)
  if("${${var}}" STREQUAL "")
    message(FATAL_ERROR "RunTest.cmake: missing -D${var}=...")
  endif()
endforeach()

# Moves the first line of the text stored in variable <text_var> into <line_var>.
function(take_line text_var line_var)
  set(t "${${text_var}}")
  string(FIND "${t}" "\n" p)
  if(p EQUAL -1)
    set(${line_var} "${t}" PARENT_SCOPE)
    set(${text_var} "" PARENT_SCOPE)
  else()
    string(SUBSTRING "${t}" 0 ${p} l)
    math(EXPR p1 "${p} + 1")
    string(SUBSTRING "${t}" ${p1} -1 rest)
    set(${line_var} "${l}" PARENT_SCOPE)
    set(${text_var} "${rest}" PARENT_SCOPE)
  endif()
endfunction()

# Describes the first line where two texts differ.
function(first_difference expected actual out_var)
  set(e "${expected}")
  set(a "${actual}")
  set(n 1)
  while(TRUE)
    if("${e}" STREQUAL "" AND "${a}" STREQUAL "")
      set(${out_var} "the texts differ only in the final newline" PARENT_SCOPE)
      return()
    endif()
    set(le "<no more lines>")
    set(la "<no more lines>")
    if(NOT "${e}" STREQUAL "")
      take_line(e le)
    endif()
    if(NOT "${a}" STREQUAL "")
      take_line(a la)
    endif()
    if(NOT "${le}" STREQUAL "${la}")
      set(${out_var} "first difference at line ${n}:\n  expected: ${le}\n  actual:   ${la}" PARENT_SCOPE)
      return()
    endif()
    math(EXPR n "${n} + 1")
  endwhile()
endfunction()

function(read_expected out_var)
  if("${EXPECTED}" STREQUAL "" OR NOT EXISTS "${EXPECTED}")
    message(FATAL_ERROR "missing expected file: '${EXPECTED}'")
  endif()
  file(READ "${EXPECTED}" content)
  string(REPLACE "\r\n" "\n" content "${content}")
  set(${out_var} "${content}" PARENT_SCOPE)
endfunction()

function(explain_exit_code rc err)
  if("${rc}" STREQUAL "70")
    message(FATAL_ERROR "this part of the lab is not implemented yet (TODO marker):\n${err}")
  endif()
endfunction()

if(MODE STREQUAL "tokens")
  execute_process(
    COMMAND "${EXE}" --tokens "${INPUT}"
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
  )
else()
  execute_process(
    COMMAND "${EXE}" "${INPUT}"
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
  )
endif()
string(REPLACE "\r\n" "\n" out "${out}")
string(REPLACE "\r\n" "\n" err "${err}")

if(MODE STREQUAL "tokens")
  explain_exit_code("${rc}" "${err}")
  if(NOT "${rc}" STREQUAL "0")
    message(FATAL_ERROR "`minilang --tokens` exited with '${rc}' (expected 0):\n${err}")
  endif()
  read_expected(expected)
  if(NOT "${out}" STREQUAL "${expected}")
    first_difference("${expected}" "${out}" diff)
    message(FATAL_ERROR "token stream differs from ${EXPECTED}\n${diff}\n\n--- expected ---\n${expected}\n--- actual ---\n${out}")
  endif()

elseif(MODE STREQUAL "valid")
  explain_exit_code("${rc}" "${err}")
  if(NOT "${rc}" STREQUAL "0")
    message(FATAL_ERROR "a VALID program was rejected (exit '${rc}'):\n${err}")
  endif()

elseif(MODE STREQUAL "invalid")
  explain_exit_code("${rc}" "${err}")
  if(NOT "${rc}" STREQUAL "2")
    message(FATAL_ERROR "an INVALID program must be rejected with a syntax error (exit 2), got exit '${rc}'.\nstdout: ${out}\nstderr: ${err}")
  endif()
  read_expected(expected)
  string(STRIP "${expected}" expected)
  string(REGEX MATCH ":([0-9]+):([0-9]+): error: " ignored "${err}")
  if("${CMAKE_MATCH_1}" STREQUAL "")
    message(FATAL_ERROR "the diagnostic must look like 'FILE:LINE:COL: error: MESSAGE', got:\n${err}")
  endif()
  set(actual "${CMAKE_MATCH_1}:${CMAKE_MATCH_2}")
  if(NOT "${actual}" STREQUAL "${expected}")
    message(FATAL_ERROR "syntax error reported at ${actual}, expected at ${expected} (the first token that cannot continue a valid program).\nstderr: ${err}")
  endif()

else()
  message(FATAL_ERROR "unknown MODE '${MODE}'")
endif()
