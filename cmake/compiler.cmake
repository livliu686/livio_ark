# compiler.cmake - 编译器选项配置
#
# 为本项目 target 统一设置编译选项（不影响第三方库）。
#
# 用法:
#   livio_set_compiler_options(LivioArk)

function(livio_set_compiler_options target)
    target_compile_features(${target} PUBLIC cxx_std_20)

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /utf-8
            /permissive-
            /external:W0      # 第三方 SYSTEM 头文件不产生警告
        )
        # 防止 <windows.h> 定义 min/max 宏，与 std::min/std::max 冲突
        target_compile_definitions(${target} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
        if(LIVIO_ARK_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-parameter
        )
        if(LIVIO_ARK_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
