cmake_minimum_required(VERSION 3.31)

project(HelloRodos VERSION 0.0.1 LANGUAGES CXX)

if(NOT DEFINED RODOS_PORT_NAME)
    message(
        FATAL_ERROR
        "RODOS_PORT_NAME is not defined. Please set it to the desired target platform."
    )
endif()
set(CMAKE_FIND_ROOT_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../tmp/staging/${RODOS_PORT_NAME}"
)

find_package(rodos REQUIRED)

if(RODOS_PORT_NAME STREQUAL linux-x86)
    add_executable(HelloRodos_HelloLinux)
    set_target_properties(
        HelloRodos_HelloLinux
        PROPERTIES OUTPUT_NAME HelloLinux
    )
    target_sources(HelloRodos_HelloLinux PRIVATE HelloLinux.cpp)
    target_link_libraries(HelloRodos_HelloLinux PRIVATE rodos::rodos)
endif()

if(RODOS_PORT_NAME STREQUAL eo_ce)
    add_executable(HelloRodos_HelloEoCe)
    set_target_properties(HelloRodos_HelloEoCe PROPERTIES OUTPUT_NAME HelloEoCe)
    target_sources(HelloRodos_HelloEoCe PRIVATE HelloEoCe.cpp)
    target_link_libraries(HelloRodos_HelloEoCe PRIVATE rodos::rodos)
    add_custom_command(
        TARGET HelloRodos_HelloEoCe
        POST_BUILD
        COMMAND
            "${CMAKE_OBJCOPY}" -O binary "$<TARGET_FILE:HelloRodos_HelloEoCe>"
            "$<TARGET_FILE_DIR:HelloRodos_HelloEoCe>/$<TARGET_FILE_BASE_NAME:HelloRodos_HelloEoCe>.bin"
        COMMENT
            "Calling objcopy on HelloEoCe to generate HelloEoCe.bin"
        VERBATIM
    )
    add_custom_command(
        TARGET HelloRodos_HelloEoCe
        POST_BUILD
        COMMAND arm-none-eabi-size $<TARGET_FILE:HelloRodos_HelloEoCe>
    )
endif()
