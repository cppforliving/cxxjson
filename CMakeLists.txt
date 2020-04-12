cmake_minimum_required(VERSION 3.10)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENV{LDFLAGS} $ENV{LINKFLAGS})

# configure ####################################################################

option(CMAKE_EXPORT_COMPILE_COMMANDS "" ON)

if("$ENV{CXX}" STREQUAL clang++)
    include(CMakeForceCompiler)
    cmake_force_cxx_compiler(clang++ Clang)
endif()

project(cxxjson
    VERSION 0.1
    LANGUAGES CXX)

include(GNUInstallDirs)

enable_testing()

foreach(define $ENV{DEFINES})
    add_definitions(-D${define})
endforeach()

include_directories(. ./inc)

# cxx ##########################################################################

install(DIRECTORY inc/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN *.hpp)

file(GLOB_RECURSE srcs src/*.cpp)
add_library(srclib OBJECT ${srcs})

add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:srclib>)

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_custom_target(cxx DEPENDS ${PROJECT_NAME})

# build ########################################################################

add_library(test_main test/test_main.cpp)
target_include_directories(test_main PUBLIC test)

file(GLOB_RECURSE tests
    RELATIVE ${CMAKE_SOURCE_DIR}/test
    test/*.cpp)
list(REMOVE_ITEM tests test_main.cpp)

add_custom_target(build)
foreach(suite ${tests})
    string(REPLACE ".cpp" "" target ${suite})
    string(CONCAT target "ut_" ${target})
    add_executable(${target} test/${suite} $<TARGET_OBJECTS:srclib>)
    target_link_libraries(${target} pthread test_main)
    add_test(NAME ${target} COMMAND ${target})
    set_property(TEST ${target} PROPERTY LABELS ut)
    add_dependencies(build ${target})
endforeach()

# benchmark ####################################################################

file(GLOB_RECURSE benchmarks
    RELATIVE ${CMAKE_SOURCE_DIR}/benchmark
    benchmark/*.cpp)

add_custom_target(benchmark_)
foreach(suite ${benchmarks})
    string(REPLACE ".cpp" "" target ${suite})
    string(CONCAT target "benchmark_" ${target})
    add_executable(${target} benchmark/${suite} $<TARGET_OBJECTS:srclib>)
    target_link_libraries(${target} pthread benchmark benchmark_main)
    add_test(NAME ${target} COMMAND ${target})
    set_property(TEST ${target} PROPERTY LABELS benchmark)
    add_dependencies(benchmark_ ${target})
endforeach()

# fuzzer #######################################################################

file(GLOB_RECURSE fuzzers
    RELATIVE ${CMAKE_SOURCE_DIR}/fuzzer
    fuzzer/*.cpp)

add_custom_target(fuzzer)
foreach(suite ${fuzzers})
    string(REPLACE ".cpp" "" target ${suite})
    string(CONCAT target "fuzzer_" ${target})
    add_executable(${target} fuzzer/${suite} $<TARGET_OBJECTS:srclib>)
    add_test(NAME ${target} COMMAND ${target})
    set_property(TEST ${target} PROPERTY LABELS fuzzer)
    add_dependencies(fuzzer ${target})
endforeach()
