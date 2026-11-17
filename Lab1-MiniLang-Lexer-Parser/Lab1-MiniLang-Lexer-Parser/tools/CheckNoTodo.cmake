# Fails while a student-written document still has its <TODO> placeholders. Provided: do not modify.
#
#   cmake -DFILE=<file> [-DHINT=<text>] -P tools/CheckNoTodo.cmake
cmake_minimum_required(VERSION 3.20)

if("${FILE}" STREQUAL "")
  message(FATAL_ERROR "usage: cmake -DFILE=<file> -P tools/CheckNoTodo.cmake")
endif()
if(NOT EXISTS "${FILE}")
  message(FATAL_ERROR "missing file: ${FILE}")
endif()

file(READ "${FILE}" text)
string(REGEX MATCHALL "<TODO>" placeholders "${text}")
list(LENGTH placeholders count)
if(count GREATER 0)
  message(FATAL_ERROR "${FILE} still has ${count} <TODO> placeholder(s). ${HINT}")
endif()
message(STATUS "${FILE}: no <TODO> placeholders left")
