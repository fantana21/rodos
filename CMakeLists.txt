cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(
    rodos
    VERSION 0.1.0
    LANGUAGES C CXX ASM
)

# This variable is set by project() in CMake 3.21+
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}" PROJECT_IS_TOP_LEVEL)
if (PROJECT_IS_TOP_LEVEL)
    # Developer mode enables targets and code paths in the CMake scripts that are only relevant for
    # the developers of RODOS. Targets necessary to build the project must be provided
    # unconditionally, so consumers can trivially build and package the project. The option is ON by
    # default for backward compatibility.
    option(RODOS_DEVELOPER_MODE "Enable developer mode" ON)
    if (RODOS_DEVELOPER_MODE)
        option(COVERAGE "Enable code coverage generation" OFF)
        if (COVERAGE)
            if (NOT CMAKE_BUILD_TYPE)
                set(CMAKE_BUILD_TYPE "Debug")
                message(STATUS "Coverage enabled, setting build type to Debug")
            endif ()
            include(cmake/CodeCoverage.cmake)
            append_coverage_compiler_flags()
        endif ()
    endif ()
endif ()

if (NOT DEFINED port_dir)
    message(FATAL_ERROR "port not correct (CMAKE_TOOLCHAIN_FILE, see readme)")
endif ()

#___________________________________________________________________
file(GLOB_RECURSE independent_sources
    src/independent/*.cpp
    support/support-libs/*.cpp)

add_library(rodos_rodos STATIC ${independent_sources})
add_library(rodos::rodos ALIAS rodos_rodos)
add_library(rodos ALIAS rodos_rodos)
set_target_properties(rodos_rodos PROPERTIES
    EXPORT_NAME rodos
    OUTPUT_NAME rodos
)

file(GLOB port_sources
    src/${port_dir}/*.cpp
    src/${port_dir}/*.c
    src/${port_dir}/hal/*.cpp
    src/${port_dir}/*.S)
target_sources(rodos_rodos PRIVATE ${port_sources})

target_include_directories(rodos_rodos PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/hal>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/${port_dir}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/${port_dir}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/support/support-libs>
)

target_compile_options(rodos_rodos PUBLIC -fno-exceptions)
target_compile_features(rodos_rodos PUBLIC cxx_std_20)
if (RODOS_DEVELOPER_MODE)
    target_compile_options(
        rodos_rodos PUBLIC -g -Wall -Wpedantic -Wextra -Wcast-qual -Wconversion -Wsign-conversion
        -Wfloat-conversion -Wdouble-promotion -Wnull-dereference -Wstrict-aliasing -Wno-long-long
        -Wno-cpp
    )
endif ()

option(ENABLE_RODOS_RTTI "Build RODOS with run-time type information" OFF)
if (NOT ENABLE_RODOS_RTTI)
    target_compile_options(rodos_rodos PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
endif ()

option(DISABLE_TIMEEVENTS "Disable manual TimeEvents (interrupts on specified time)" OFF)
if(DISABLE_TIMEEVENTS)
    add_compile_definitions(DISABLE_TIMEEVENTS)
endif()


#___________________________________________________________________
if (is_port_baremetal)
    file(GLOB baremetal_generic_sources src/bare-metal-generic/*.cpp)
    target_sources(rodos_rodos PRIVATE ${baremetal_generic_sources})
    target_include_directories(rodos_rodos PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/bare-metal-generic>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/bare-metal-generic/ip/lwip/include>)

    option(DISABLE_SLEEP_WHEN_IDLE "Do not go to sleep mode when idling" OFF)
    if(DISABLE_SLEEP_WHEN_IDLE)
        add_compile_definitions(DISABLE_SLEEP_WHEN_IDLE)
    endif()
endif ()


#___________________________________________________________________
#    optional extra files which are port dependent. See cmake/port/xxx.cmake
#___________________________________________________________________
if (compile_definitions)
  target_compile_definitions(rodos_rodos PUBLIC ${compile_definitions})
endif ()

if (compile_options)
  target_compile_options(rodos_rodos PUBLIC ${compile_options})
endif ()

if (link_options)
  target_link_options(rodos_rodos PUBLIC ${link_options})
endif ()

if (sources_to_add)
    file(GLOB files ${sources_to_add})
    target_sources(rodos_rodos PRIVATE ${files})
endif ()


if (directories_to_include)
    foreach(dir ${directories_to_include})
        target_include_directories(rodos_rodos PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>)
    endforeach()
endif ()

if (directories_to_link)
  target_link_directories(rodos_rodos PUBLIC ${directories_to_link})
endif ()

if (libraries_to_link)
    target_link_libraries(rodos_rodos PUBLIC ${libraries_to_link})
endif ()

if (linker_script)
  target_link_options(rodos_rodos PUBLIC -T${linker_script})
endif()

option(EXECUTABLE "Build executables for RODOS tutorials" OFF)
if (EXECUTABLE)
    include(cmake/executable.cmake)
    add_subdirectory(tutorials)
endif ()


# Install rules
include(cmake/install-rules.cmake)


if (RODOS_DEVELOPER_MODE)
    if (COVERAGE)
        setup_lcov_coverage(
            NAME coverage
            LCOV_ARGS "--no-external")
        message("Generate coverage report with 'make coverage_collect'")
        add_custom_command(TARGET coverage_collect POST_BUILD
            COMMAND sed -ie "s@SF:${CMAKE_CURRENT_SOURCE_DIR}/\\(.*\\)@SF:\\1@g"
            ${CMAKE_CURRENT_BINARY_DIR}/coverage.info
            COMMENT "Changing paths in coverage.info to relative paths."
            VERBATIM
        )
    endif ()
    enable_testing()
    add_subdirectory(test-suite)

    option(UNIT_TESTS "Enable building unit tests" OFF)
    if (UNIT_TESTS)
        add_subdirectory(support/test)
    endif ()
endif ()
