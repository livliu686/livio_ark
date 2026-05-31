cmake_minimum_required(VERSION 4.2)
project(LivioArk VERSION 0.0.0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# cmake 工具集
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/options.cmake)
include(cmake/compiler.cmake)
include(cmake/utils.cmake)

# 依赖（spdlog 等）
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)

# LivioArk 库
if(LIVIO_ARK_BUILD_SHARED)
    add_library(LivioArk SHARED src/log/log.cpp)
    target_compile_definitions(LivioArk PRIVATE LIVIO_ARK_BUILD_SHARED)
else()
    add_library(LivioArk STATIC src/log/log.cpp)
endif()

livio_set_compiler_options(LivioArk)
livio_set_output_dirs(LivioArk)

target_include_directories(LivioArk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# spdlog 作为 PRIVATE 依赖：仅在实现文件内部使用，不对外暴露头文件
target_link_libraries(LivioArk PRIVATE spdlog::spdlog)

# 示例
if(LIVIO_ARK_BUILD_EXAMPLES)
    livio_add_example(log_example examples/log_example.cpp)
endif()

# 测试
if(LIVIO_ARK_BUILD_TESTS)
    enable_testing()
    include(cmake/testing.cmake)
    add_subdirectory(tests)
endif()

livio_print_config()
