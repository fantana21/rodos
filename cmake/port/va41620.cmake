if(NOT board)
    message(FATAL_ERROR "Do not use va41620 directly; instead, use a board port file.")
endif()

# Toolchain

set(CMAKE_SYSTEM_NAME Generic)

set(toolchain_prefix   arm-none-eabi-)
set(CMAKE_ASM_COMPILER ${toolchain_prefix}gcc)
set(CMAKE_C_COMPILER   ${toolchain_prefix}gcc)
set(CMAKE_CXX_COMPILER ${toolchain_prefix}g++)
set(CMAKE_SIZE         ${toolchain_prefix}size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Platform

set(compile_and_link_options
    -mcpu=cortex-m4
    -mfloat-abi=softfp
    -mfpu=fpv4-sp-d16
    -specs=nano.specs
)
set(compile_options
    ${compile_and_link_options}
    -gdwarf-2
    -mthumb
)
set(link_options
    ${compile_and_link_options}
    -nostartfiles
    -specs=nosys.specs
    -Xlinker --gc-sections
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

# RODOS

set(port_dir "bare-metal/va41620")
set(is_port_baremetal TRUE)

set(RODOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(linker_script ${RODOS_DIR}/src/bare-metal/va41620/scripts/linkerscript.ld)

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
