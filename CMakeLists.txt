cmake_minimum_required(VERSION 2.8)

project(Demo)

# Compiler-specific flags and definitions
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
endif()

include_directories("${PROJECT_SOURCE_DIR}/source")

# This is only required for the script to work in the version control
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

set(SOURCE_PATH "${PROJECT_SOURCE_DIR}/source")
set(SOURCES
    "${SOURCE_PATH}/tsetlin/TsetlinMachine.h"
    "${SOURCE_PATH}/tsetlin/TsetlinMachine.cpp"
    "${SOURCE_PATH}/Main.cpp"
)

add_executable(Demo ${SOURCES})