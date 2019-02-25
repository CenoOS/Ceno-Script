cmake_minimum_required(VERSION 3.8)
project(Ceno_Script)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/ceno_script.c)

add_executable(Ceno_Script ${SOURCE_FILES})