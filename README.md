# SimpleJucePluginTemplate

## Installation

Install dependencies that are managed using `conan`:
```shell
conan install . --build=missing -s build_type=Release
conan install . --build=missing -s build_type=Debug
```

### CMake

Configure the project:
```shell
# cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

The build:
```shell
cmake --build build
```