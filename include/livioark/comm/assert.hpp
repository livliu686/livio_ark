#pragma once

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <source_location>

// ==============================================
// 调试与断言宏
// ==============================================

// 运行时断言（Debug 生效，Release 失效）
#ifdef NDEBUG
#    define LIVIO_ARK_ASSERT(expr)          ((void)0)
#    define LIVIO_ARK_ASSERT_MSG(expr, msg) ((void)0)
#else
#    define LIVIO_ARK_ASSERT(expr) assert(expr)
#    define LIVIO_ARK_ASSERT_MSG(expr, msg)                        \
        do                                                         \
        {                                                          \
            if (!(expr))                                           \
            {                                                      \
                const auto& loc = std::source_location::current(); \
                fprintf(stderr,                                    \
                        "[ASSERT] %s:%d: %s: %s\n",                \
                        loc.file_name(),                           \
                        loc.line(),                                \
                        loc.function_name(),                       \
                        msg);                                      \
                std::abort();                                      \
            }                                                      \
        } while (0)
#endif

// 编译期断言
#define LIVIO_ARK_STATIC_ASSERT(expr, msg) static_assert(expr, msg)

// Debug 专属代码块
#ifdef NDEBUG
#    define LIVIO_ARK_DEBUG_ONLY(code) ((void)0)
#else
#    define LIVIO_ARK_DEBUG_ONLY(code) \
        do                             \
        {                              \
            code;                      \
        } while (0)
#endif
