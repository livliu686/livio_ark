# spdlog 依赖：使用项目内 thirdparty/spdlog（不联网拉取）
# 以静态库形式编译进 LivioArk，对外不可见 spdlog 符号/头文件
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/spdlog ${CMAKE_BINARY_DIR}/thirdparty/spdlog)
