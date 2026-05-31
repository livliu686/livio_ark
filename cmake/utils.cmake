# utils.cmake - 工具函数

# 统一设置项目输出目录（仅对本项目 target 使用，不影响第三方库）
#
#   <build>/output/bin/    可执行文件
#   <build>/output/lib/    静态库 / 动态库
#   <build>/output/test/   测试可执行文件
#
# 用法:
#   livio_set_output_dirs(LivioArk)            → output/lib/
#   livio_set_output_dirs(log_example)         → output/bin/
#   livio_set_output_dirs(test_log TEST)       → output/test/
#
set(LIVIO_ARK_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output" CACHE PATH "Project output root")

function(livio_set_output_dirs target)
    set(_is_test FALSE)
    if("TEST" IN_LIST ARGN)
        set(_is_test TRUE)
    endif()

    get_target_property(_type ${target} TYPE)

    if(_is_test)
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${LIVIO_ARK_OUTPUT_DIR}/test"
        )
    elseif(_type STREQUAL "EXECUTABLE")
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${LIVIO_ARK_OUTPUT_DIR}/bin"
        )
    else()
        set_target_properties(${target} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${LIVIO_ARK_OUTPUT_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${LIVIO_ARK_OUTPUT_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${LIVIO_ARK_OUTPUT_DIR}/bin"
        )
    endif()
endfunction()

# 收集指定目录下所有源文件
#
# 用法:
#   livio_collect_sources(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_FILES)
function(livio_collect_sources dir out_var)
    file(GLOB_RECURSE _sources
        ${dir}/*.cpp
        ${dir}/*.cc
        ${dir}/*.h
        ${dir}/*.hpp
    )
    set(${out_var} ${_sources} PARENT_SCOPE)
endfunction()

# 添加一个示例可执行文件
#
# 用法:
#   livio_add_example(log_example examples/log_example.cpp)
function(livio_add_example name)
    add_executable(${name} ${ARGN})
    livio_set_compiler_options(${name})
    livio_set_output_dirs(${name})
    target_link_libraries(${name} PRIVATE LivioArk)
endfunction()

# 打印构建信息
function(livio_print_config)
    message(STATUS "=== LivioArk Build Configuration ===")
    message(STATUS "  Version:      ${PROJECT_VERSION}")
    message(STATUS "  Build type:   ${CMAKE_BUILD_TYPE}")
    message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "  Shared lib:   ${LIVIO_ARK_BUILD_SHARED}")
    message(STATUS "  Examples:     ${LIVIO_ARK_BUILD_EXAMPLES}")
    message(STATUS "  Tests:        ${LIVIO_ARK_BUILD_TESTS}")
    message(STATUS "====================================")
endfunction()
