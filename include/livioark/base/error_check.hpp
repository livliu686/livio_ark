#pragma once

// ==============================================
// 安全与错误处理宏
// ==============================================

// 强制检查（Release 也生效）
#define LIVIO_ARK_CHECK(expr) \
    do { \
        if (LIVIO_ARK_UNLIKELY(!(expr))) { \
            const auto& loc = std::source_location::current(); \
            fprintf(stderr, "[CHECK] %s:%d: %s: Check failed: %s\n", \
                loc.file_name(), loc.line(), loc.function_name(), #expr); \
            std::abort(); \
        } \
    } while(0)

#define LIVIO_ARK_CHECK_NOTNULL(ptr) \
    LIVIO_ARK_CHECK((ptr) != nullptr)

// 简化错误处理
#define LIVIO_ARK_TRY_OR_RETURN(expr, err) \
    do { \
        auto res = (expr); \
        if (LIVIO_ARK_UNLIKELY(!res)) { \
            return (err); \
        } \
    } while(0)
