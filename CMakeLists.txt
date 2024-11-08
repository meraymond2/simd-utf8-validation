cmake_minimum_required(VERSION 3.0)
project(utf8_validation C)

set(CMAKE_C_STANDARD 17)

add_compile_options(-mavx512f)

add_executable(utf8_validation main.c branchy.c
        branchy.h
        fsm.c
        fsm.h
        lookup.c
        lookup.h)
