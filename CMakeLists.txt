cmake_minimum_required(VERSION 2.6)

project("Émulateur PROCSI")
set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_C_FLAGS "-Wall -Werror -std=c99 -D_POSIX_C_SOURCE=2")

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile")
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG})
endif(DOXYGEN_FOUND)

file(GLOB_RECURSE src_files src/*)

message(STATUS ${src_files})

add_executable(main ${src_files})

add_custom_target(run ${EXECUTABLE_OUTPUT_PATH}/main)
