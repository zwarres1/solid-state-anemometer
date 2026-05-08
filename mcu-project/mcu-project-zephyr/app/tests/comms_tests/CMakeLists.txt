cmake_minimum_required(VERSION 3.20.0)

get_filename_component(APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(comms_tests)

target_sources(app PRIVATE 
    src/main.cpp 
    ${APP_ROOT}/src/comms/protocol.cpp 
)

target_include_directories(app PRIVATE 
    ${APP_ROOT}/src/comms
)