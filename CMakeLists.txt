cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)

project(multirun VERSION 1.1.2 LANGUAGES C)

add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")

add_executable(multirun multirun.c)

target_include_directories(multirun PUBLIC "${PROJECT_BINARY_DIR}")

install(TARGETS multirun DESTINATION bin)
