#pragma once

// ==============================================
// 基础通用宏
// ==============================================

// 命名空间包裹
#define LIVIO_ARK_NAMESPACE_BEGIN \
    namespace livio::ark          \
    {
#define LIVIO_ARK_NAMESPACE_END }

// 禁止拷贝/移动
#define LIVIO_ARK_DISABLE_COPY(ClassName)            \
    ClassName(const ClassName&)            = delete; \
    ClassName& operator=(const ClassName&) = delete

#define LIVIO_ARK_DISABLE_MOVE(ClassName)       \
    ClassName(ClassName&&)            = delete; \
    ClassName& operator=(ClassName&&) = delete

#define LIVIO_ARK_DISABLE_COPY_MOVE(ClassName) \
    LIVIO_ARK_DISABLE_COPY(ClassName)          \
    LIVIO_ARK_DISABLE_MOVE(ClassName)

// 未使用变量/参数标记
#define LIVIO_ARK_UNUSED(x) (void)(x)

// 函数属性
#define LIVIO_ARK_NORETURN     [[noreturn]]
#define LIVIO_ARK_NODISCARD    [[nodiscard]]
#if defined(_MSC_VER)
#    define LIVIO_ARK_FORCE_INLINE __forceinline
#    define LIVIO_ARK_NO_INLINE    __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#    define LIVIO_ARK_FORCE_INLINE inline __attribute__((always_inline))
#    define LIVIO_ARK_NO_INLINE    __attribute__((noinline))
#else
#    define LIVIO_ARK_FORCE_INLINE inline
#    define LIVIO_ARK_NO_INLINE
#endif

// 分支预测优化（表达式上下文可用）
#if defined(__GNUC__) || defined(__clang__)
#    define LIVIO_ARK_LIKELY(x)   __builtin_expect(!!(x), 1)
#    define LIVIO_ARK_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#    define LIVIO_ARK_LIKELY(x)   (x)
#    define LIVIO_ARK_UNLIKELY(x) (x)
#endif

// 标识符拼接
#define LIVIO_ARK_CONCAT_IMPL(a, b) a##b
#define LIVIO_ARK_CONCAT(a, b)      LIVIO_ARK_CONCAT_IMPL(a, b)
