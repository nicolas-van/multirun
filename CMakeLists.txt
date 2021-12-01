cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)

# set the project name
project(multirun VERSION 1.0.1 LANGUAGES C)

configure_file(config.h.in config.h)

add_executable(multirun multirun.c)

target_include_directories(multirun PUBLIC "${PROJECT_BINARY_DIR}")

install(TARGETS multirun DESTINATION bin)
