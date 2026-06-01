cmake_minimum_required(VERSION 4.2)
project(LivioArk VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# cmake 工具集
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/options.cmake)
include(cmake/compiler.cmake)
include(cmake/utils.cmake)

# 依赖（spdlog 等）
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)

# 版本头文件：以 project(VERSION) 为单一真相源，生成 version.hpp
set(LIVIO_ARK_GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/include)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in
    ${LIVIO_ARK_GENERATED_INCLUDE_DIR}/livioark/comm/version.hpp
    @ONLY
)

# LivioArk 库
if(LIVIO_ARK_BUILD_SHARED)
    add_library(LivioArk SHARED src/log/log.cpp)
    # 仅在构建动态库本身时导出符号（PRIVATE，不传播给消费者）
    target_compile_definitions(LivioArk PRIVATE LIVIO_ARK_EXPORTS)
    # 非 Windows 下默认隐藏符号，仅 LIVIO_ARK_API 标注的符号可见
    set_target_properties(LivioArk PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
else()
    add_library(LivioArk STATIC src/log/log.cpp)
    # 静态库：构建与消费时都不加导出/导入修饰（PUBLIC 传播给消费者）
    target_compile_definitions(LivioArk PUBLIC LIVIO_ARK_STATIC)
endif()

livio_set_compiler_options(LivioArk)
livio_set_output_dirs(LivioArk)

target_include_directories(LivioArk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${LIVIO_ARK_GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# spdlog 作为 PRIVATE 依赖：构建树链接 spdlog::spdlog，安装树链接导出的 LivioArk::spdlog
target_link_libraries(LivioArk PRIVATE
    $<BUILD_INTERFACE:spdlog::spdlog>
    $<INSTALL_INTERFACE:LivioArk::spdlog>
)

# 示例
if(LIVIO_ARK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 测试
if(LIVIO_ARK_BUILD_TESTS)
    enable_testing()
    include(cmake/testing.cmake)
    add_subdirectory(tests)
endif()

if(LIVIO_ARK_ENABLE_INSTALL)
    # 安装与包导出（支持 find_package(LivioArk CONFIG)）
    install(TARGETS LivioArk
        EXPORT LivioArkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    if(TARGET spdlog)
        install(TARGETS spdlog
            EXPORT LivioArkTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
    endif()

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(FILES
        ${LIVIO_ARK_GENERATED_INCLUDE_DIR}/livioark/comm/version.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/livioark/comm
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LivioArkConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LivioArkConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LivioArk
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LivioArkConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/LivioArkConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/LivioArkConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LivioArk
    )

    install(EXPORT LivioArkTargets
        FILE LivioArkTargets.cmake
        NAMESPACE LivioArk::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LivioArk
    )
endif()

livio_print_config()
