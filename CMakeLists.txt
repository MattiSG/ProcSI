cmake_minimum_required(VERSION 2.6)

project("Émulateur PROCSI")
set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_C_FLAGS "-Wall -Werror -std=c99 -g -D_POSIX_C_SOURCE=2")

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile")
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG})
endif(DOXYGEN_FOUND)

file(GLOB_RECURSE src_files src/*)

add_executable(main ${src_files})

add_custom_target(run ${EXECUTABLE_OUTPUT_PATH}/main)

#enable_testing()
#include_directories(src/)
#set(TEST_DIR test)
#set(TEST_FILES breakpoint)
#foreach(filename ${TEST_FILES})
#    add_executable(test_${filename} ${TEST_DIR}/test_${filename}.c)
#    add_test(${filename} ${EXECUTABLE_OUTPUT_PATH}/test_${filename})
#endforeach(filename)
