# testing.cmake - 测试工具函数
#
# 使用 GoogleTest + CTest。GoogleTest 解析顺序：
#   1) thirdparty/googletest（项目内置，离线可用）
#   2) find_package(GTest)（系统/包管理器安装）
#   3) FetchContent 在线拉取（需联网，可用 LIVIO_ARK_GTEST_TAG 指定版本）
# 全部失败时，关闭测试构建并给出提示。

include(CTest)

# 允许通过缓存变量覆盖：是否允许在线拉取、拉取的 git 仓库与 tag
option(LIVIO_ARK_FETCH_GTEST "Allow fetching GoogleTest online when not found locally" ON)
set(LIVIO_ARK_GTEST_REPOSITORY "https://gitee.com/mirrors/googletest.git"
    CACHE STRING "GoogleTest git repository URL used by FetchContent")
set(LIVIO_ARK_GTEST_TAG "v1.15.2" CACHE STRING "GoogleTest git tag used by FetchContent")

set(_livio_gtest_available FALSE)

# gtest 公共构建选项（内置与在线拉取共用）
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/googletest/CMakeLists.txt")
    # 1) 项目内置，离线可用
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/googletest
                     ${CMAKE_BINARY_DIR}/thirdparty/googletest)
    set(_livio_gtest_available TRUE)
else()
    # 2) 系统/包管理器安装
    find_package(GTest CONFIG QUIET)
    if(GTest_FOUND)
        set(_livio_gtest_available TRUE)
    elseif(LIVIO_ARK_FETCH_GTEST)
        # 3) FetchContent 在线拉取
        include(FetchContent)
        message(STATUS "GoogleTest not found locally, fetching ${LIVIO_ARK_GTEST_TAG} via FetchContent...")
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY ${LIVIO_ARK_GTEST_REPOSITORY}
            GIT_TAG        ${LIVIO_ARK_GTEST_TAG}
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(googletest)
        if(TARGET gtest)
            # 统一别名，保证 GTest::* 命名空间目标可用
            if(NOT TARGET GTest::gtest)
                add_library(GTest::gtest ALIAS gtest)
            endif()
            if(NOT TARGET GTest::gtest_main)
                add_library(GTest::gtest_main ALIAS gtest_main)
            endif()
            if(TARGET gmock AND NOT TARGET GTest::gmock)
                add_library(GTest::gmock ALIAS gmock)
            endif()
            if(TARGET gmock_main AND NOT TARGET GTest::gmock_main)
                add_library(GTest::gmock_main ALIAS gmock_main)
            endif()
            set(_livio_gtest_available TRUE)
        endif()
    endif()
endif()

if(NOT _livio_gtest_available)
    message(WARNING
        "GoogleTest not found (no thirdparty/googletest, find_package(GTest) failed, "
        "and online fetch disabled or unavailable). Tests will be skipped.")
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
