cmake_minimum_required(VERSION 3.22)
project(WordChecker C)
set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-DEVAL -Wall -Werror -std=gnu11 -O2 -pipe -static -s -g3 -lm")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
add_executable(WordChecker
        src/main.c)
