cmake_minimum_required(VERSION 3.20)

project(ShellGuesser LANGUAGES C)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(FATAL_ERROR "This program cannot be compiled on Windows.")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)

file(
    GLOB_RECURSE ShellCheckFiles
    "${CMAKE_CURRENT_SOURCE_DIR}/include/shell-check.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/shell-check/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/shell-check/*.c"
)

include(GenerateExportHeader)

add_library(ShellCheck "${ShellCheckFiles}")

set(ShellCheckIncludePath "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(ShellCheck PUBLIC "${ShellCheckIncludePath}")


generate_export_header(ShellCheck
    EXPORT_MACRO_NAME SC_EXPORT
)

add_executable(ShellGuesser "${CMAKE_CURRENT_SOURCE_DIR}/guesser/main.c")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

target_link_libraries(ShellGuesser PUBLIC $<TARGET_OBJECTS:ShellCheck> ${GTK3_LIBRARIES})

target_include_directories(ShellGuesser PUBLIC "${ShellCheckIncludePath}" "${GTK3_INCLUDE_DIRS}")
