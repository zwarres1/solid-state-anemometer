# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(
    solid-state-anemometer
    VERSION 1.0
    LANGUAGES C CXX
)

zephyr_include_directories(include)

target_sources(app PRIVATE src/main.cpp)

add_subdirectory(src/comms)
add_subdirectory(src/adc)
add_subdirectory(src/calculations)