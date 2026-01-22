## Linux x86

~~~shell
cmake -S . -B build/linux-x86 -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DRODOS_PORT_NAME=linux-x86
cmake --build build/linux-x86
cmake --install build/linux-x86 --prefix ./tmp/staging/linux-x86
~~~


## EO CE/VA41620

~~~shell
cmake -S . -B build/eo_ce -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DRODOS_PORT_NAME=eo_ce
cmake --build build/eo_ce
cmake --install build/eo_ce --prefix ./tmp/staging/eo_ce
~~~
