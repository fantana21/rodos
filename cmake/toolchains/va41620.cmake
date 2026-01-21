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
    -ffunction-sections
    -fdata-sections
)
set(link_options
    ${compile_and_link_options}
    -nostartfiles
    -specs=nosys.specs
    -Xlinker --gc-sections
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)
