set(port_dir "bare-metal/linux-x86")
set(is_port_baremetal TRUE)

# Set a default toolchain file
set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../toolchains/linux-x86.cmake"
    CACHE FILEPATH
    "Toolchain file for cross-compiling"
)
