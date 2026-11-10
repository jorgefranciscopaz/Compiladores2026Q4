cmake_minimum_required(VERSION 3.20)
project(Compiler_v1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} 
src/main.cpp 
src/Lexer.cpp 
src/Lexer.hpp 
src/Parser.hpp
src/Parser.cpp
src/LL1.hpp
src/LL1.cpp)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
