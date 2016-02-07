cmake_minimum_required (VERSION 3.0)
project(libcparse)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions("-DUNIX")
    add_definitions("-DLINUX")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions("-DUNIX")
    add_definitions("-DDARWIN")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions("-DWINDOWS")
endif()

if (NOT MSVC)
    add_definitions("-Wall")
    add_definitions("-Wextra")
    add_definitions("-g")
endif()


aux_source_directory(src SRCS)
include_directories(src)

set(SRCS_LIB ${SRCS})

add_library(cparse_static STATIC ${SRCS_LIB})
set_target_properties(cparse_static PROPERTIES OUTPUT_NAME "cparse")
set_target_properties(cparse_static PROPERTIES LINKER_LANGUAGE C)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)



