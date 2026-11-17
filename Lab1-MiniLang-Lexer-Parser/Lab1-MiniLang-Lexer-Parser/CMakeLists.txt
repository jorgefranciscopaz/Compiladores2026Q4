cmake_minimum_required(VERSION 3.20)
project(MiniLangLab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(minilang
    src/Main.cpp
    src/Lexer.cpp
    src/Lexer.hpp
    src/Parser.cpp
    src/Parser.hpp
    src/NotImplemented.hpp
)

if(MSVC)
  target_compile_options(minilang PRIVATE /W4)
else()
  target_compile_options(minilang PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------------------------------------
# Tests (run them with:  ctest --test-dir build --output-on-failure)
#
#   lexer.*            tests/lexer/<name>.txt             + <name>.tokens    (exact token stream)
#   parser.valid.*     tests/parser/valid/<name>.txt                         (must be accepted)
#   parser.invalid.*   tests/parser/invalid/<name>.txt    + <name>.expected  (rejected at LINE:COL)
#   example.*          examples/valid|invalid/...         (same rules as parser.*)
#   student.*          tests/student/...                  (YOUR tests: same rules, auto-registered)
#   docs.*             your DFA/grammar documents and your spec
#
# Select a group with a label:  ctest --test-dir build -L lexer   (lexer | parser | examples | student | docs)
# -----------------------------------------------------------------------------------------
enable_testing()

set(RUN_TEST "${CMAKE_CURRENT_SOURCE_DIR}/tests/RunTest.cmake")

function(minilang_test NAME MODE INPUT EXPECTED LABEL)
  add_test(NAME ${NAME}
    COMMAND ${CMAKE_COMMAND}
      -DEXE=$<TARGET_FILE:minilang> -DMODE=${MODE} -DINPUT=${INPUT} -DEXPECTED=${EXPECTED}
      -P ${RUN_TEST})
  set_tests_properties(${NAME} PROPERTIES LABELS "${LABEL}" TIMEOUT 10)
endfunction()

# Registers every <dir>/*.txt. For MODE tokens/invalid the companion file <name>.<EXT> is required.
function(register_dir DIR PREFIX MODE EXT LABEL)
  file(GLOB inputs CONFIGURE_DEPENDS "${DIR}/*.txt")
  foreach(input IN LISTS inputs)
    get_filename_component(name "${input}" NAME_WE)
    set(expected "")
    if(NOT "${EXT}" STREQUAL "")
      set(expected "${DIR}/${name}.${EXT}")
      if(NOT EXISTS "${expected}")
        message(WARNING "${input}: missing companion file ${name}.${EXT}, test not registered")
        continue()
      endif()
    endif()
    minilang_test("${PREFIX}.${name}" ${MODE} "${input}" "${expected}" "${LABEL}")
  endforeach()
endfunction()

set(T "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(E "${CMAKE_CURRENT_SOURCE_DIR}/examples")

register_dir("${T}/lexer"                   lexer            tokens  tokens   lexer)
register_dir("${T}/parser/valid"            parser.valid     valid   ""       parser)
register_dir("${T}/parser/invalid"          parser.invalid   invalid expected parser)
register_dir("${E}/valid"                   example.valid    valid   ""       examples)
register_dir("${E}/invalid"                 example.invalid  invalid expected examples)
register_dir("${T}/student/lexer"           student.lexer    tokens  tokens   student)
register_dir("${T}/student/parser/valid"    student.valid    valid   ""       student)
register_dir("${T}/student/parser/invalid"  student.invalid  invalid expected student)

# Minimum number of tests you must add yourself (Lab1.md, section 8)
set(STUDENT_MIN_LEXER 5)
set(STUDENT_MIN_VALID 3)
set(STUDENT_MIN_INVALID 5)
add_test(NAME student.minimum_tests
  COMMAND ${CMAKE_COMMAND} -DDIR=${T}/student
    -DMIN_LEXER=${STUDENT_MIN_LEXER} -DMIN_VALID=${STUDENT_MIN_VALID} -DMIN_INVALID=${STUDENT_MIN_INVALID}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/CheckStudentTests.cmake)
set_tests_properties(student.minimum_tests PROPERTIES LABELS "student;docs")

# Documentation deliverables ------------------------------------------------------------------
find_program(REFLEX_EXECUTABLE reflex)
if(REFLEX_EXECUTABLE)
  add_test(NAME docs.spec
    COMMAND ${CMAKE_COMMAND} -DREFLEX=${REFLEX_EXECUTABLE}
      -DSPEC=${CMAKE_CURRENT_SOURCE_DIR}/spec/MiniLang.l -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/spec-check
      -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/CheckSpec.cmake)
  set_tests_properties(docs.spec PROPERTIES LABELS docs)
else()
  message(STATUS "reflex not found: test docs.spec is not registered (install RE/flex, see Lab1.md section 4)")
endif()

add_test(NAME docs.traceability
  COMMAND ${CMAKE_COMMAND} -DLAB_DIR=${CMAKE_CURRENT_SOURCE_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/CheckTraceability.cmake)
set_tests_properties(docs.traceability PROPERTIES LABELS docs)

add_test(NAME rules.no_generators
  COMMAND ${CMAKE_COMMAND} -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/CheckRules.cmake)
set_tests_properties(rules.no_generators PROPERTIES LABELS docs)

add_test(NAME docs.ll1_analysis
  COMMAND ${CMAKE_COMMAND} -DFILE=${CMAKE_CURRENT_SOURCE_DIR}/docs/LL1-ANALYSIS.md
    "-DHINT=Complete docs/LL1-ANALYSIS.md (Lab1.md, steps B1 and B2)."
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/CheckNoTodo.cmake)
set_tests_properties(docs.ll1_analysis PROPERTIES LABELS docs)
