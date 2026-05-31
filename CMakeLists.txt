cmake_minimum_required(VERSION 4.2)
project(LivioArk VERSION 0.0.0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 依赖（spdlog 等）
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)

# LivioArk 共享库
add_library(LivioArk SHARED
    src/log/log.cpp
)

target_include_directories(LivioArk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# spdlog 作为 PRIVATE 依赖：仅在实现文件内部使用，不对外暴露头文件
target_link_libraries(LivioArk PRIVATE spdlog::spdlog)

# 导出宏配置
target_compile_definitions(LivioArk PRIVATE LIVIO_ARK_BUILD_SHARED)

# 示例
option(LIVIO_ARK_BUILD_EXAMPLES "Build examples" ON)
if(LIVIO_ARK_BUILD_EXAMPLES)
    add_executable(log_example examples/log_example.cpp)
    target_link_libraries(log_example PRIVATE LivioArk)
endif()
