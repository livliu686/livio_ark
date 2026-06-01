# LivioArk

LivioArk is a C++20 utility library focused on practical infrastructure components:
- structured logging (`livioark/log/log.hpp`)
- lightweight common macros/utilities (`livioark/comm/*`)
- header-first message bus (`livioark/msgbus/*`)

## Requirements

- CMake 4.2+
- C++20 compiler
- Ninja (recommended)

## Build

Static build (default):

```bash
cmake -S . -B build -G Ninja -DLIVIO_ARK_BUILD_SHARED=OFF
cmake --build build
```

Shared build:

```bash
cmake -S . -B build -G Ninja -DLIVIO_ARK_BUILD_SHARED=ON
cmake --build build
```

## Test

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

GoogleTest resolution order is:
1. `thirdparty/googletest`
2. `find_package(GTest)`
3. `FetchContent` (`LIVIO_ARK_GTEST_REPOSITORY` + `LIVIO_ARK_GTEST_TAG`)

## CMake Presets

Predefined presets are provided in `CMakePresets.json`:
- `debug-static`
- `debug-shared`
- `release-static`
- `release-shared`

Example:

```bash
cmake --preset debug-static
cmake --build --preset build-debug-static
ctest --preset test-debug-static
```
