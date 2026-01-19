cmake_minimum_required(VERSION 3.17)
project(library_development)

set(CMAKE_CXX_STANDARD 14)

include_directories(..)

add_compile_options(-D ENABLE_TEST_CODE)

add_executable(library_development
        ../target_based_on_time.cpp
        ../target_based_on_time.hpp
        target_based_on_time_debug.cpp)
