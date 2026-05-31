# testing.cmake - 测试工具函数
#
# 使用 GoogleTest + CTest。GoogleTest 解析顺序：
#   1) thirdparty/googletest（项目内置，离线可用）
#   2) find_package(GTest)（系统/包管理器安装）
# 两者都找不到时，关闭测试构建并给出提示。

include(CTest)

set(_livio_gtest_available FALSE)

if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/googletest/CMakeLists.txt")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/googletest
                     ${CMAKE_BINARY_DIR}/thirdparty/googletest)
    set(_livio_gtest_available TRUE)
else()
    find_package(GTest CONFIG QUIET)
    if(GTest_FOUND)
        set(_livio_gtest_available TRUE)
    endif()
endif()

if(NOT _livio_gtest_available)
    message(WARNING
        "GoogleTest not found (no thirdparty/googletest and find_package(GTest) failed). "
        "Tests will be skipped. Place GoogleTest in thirdparty/googletest to enable them.")
endif()

# 添加一个测试可执行文件并自动注册到 CTest
#
# 用法:
#   livio_add_test(
#       NAME    test_log
#       SOURCES tests/test_log.cpp
#       LIBS    LivioArk
#   )
function(livio_add_test)
    cmake_parse_arguments(ARG "" "NAME" "SOURCES;LIBS" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "livio_add_test: NAME is required")
    endif()
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "livio_add_test: SOURCES is required")
    endif()
    if(NOT _livio_gtest_available)
        message(STATUS "Skipping test '${ARG_NAME}' (GoogleTest unavailable)")
        return()
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES})
    livio_set_compiler_options(${ARG_NAME})
    livio_set_output_dirs(${ARG_NAME} TEST)

    target_link_libraries(${ARG_NAME} PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${ARG_LIBS}
    )

    # 自动发现并注册测试用例（PRE_TEST 模式推迟到 ctest 运行时，避免构建期依赖 DLL）
    include(GoogleTest)
    gtest_discover_tests(${ARG_NAME}
        WORKING_DIRECTORY ${LIVIO_ARK_OUTPUT_DIR}/test
        DISCOVERY_MODE    PRE_TEST
    )
endfunction()
