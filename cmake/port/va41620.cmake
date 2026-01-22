if(NOT board)
    message(FATAL_ERROR "Do not use va41620 directly; instead, use a board port file.")
endif()

set(RODOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(linker_script ${RODOS_DIR}/src/bare-metal/va41620/scripts/linkerscript.ld)

set(port_dir "bare-metal/va41620")
set(is_port_baremetal TRUE)

if(NOT DEFINED RODOS_PLL_TARGET_FREQUENCY)
    set(RODOS_PLL_TARGET_FREQUENCY 100000000)
    message("Did not get RODOS_PLL_TARGET_FREQUENCY override, using default: ${RODOS_PLL_TARGET_FREQUENCY}")
else()
    message("RODOS_PLL_TARGET_FREQUENCY used ${RODOS_PLL_TARGET_FREQUENCY}")
endif()

set(compile_definitions
    ATOMIC_VARIANT=ATOMIC_VARIANT_STD_FALLBACK_CUSTOM
    PLL_TARGET_FREQUENCY=${RODOS_PLL_TARGET_FREQUENCY}
)
set(sources_to_add
    ${RODOS_DIR}/src/bare-metal/va41620/hw/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/startup/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/cortex_m/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/peripheral_ctrl/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/peripheral_ctrl/clock_management/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/peripheral_ctrl/pin_config/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/hal/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/hw_hal/*.cpp
    ${RODOS_DIR}/src/bare-metal/va41620/hw_hal/can/*.cpp
)
set(directories_to_include
    src/bare-metal/va41620/api_includes
    src/bare-metal/va41620/subtargets/${board}
)
set(libraries_to_link
    m
)

# Set a default toolchain file
set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../toolchains/va41620.cmake"
    CACHE FILEPATH
    "Toolchain file for cross-compiling"
)
