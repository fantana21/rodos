set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

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
