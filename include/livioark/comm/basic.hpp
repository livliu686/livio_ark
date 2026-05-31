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
#define LIVIO_ARK_FORCE_INLINE [[gnu::always_inline]] inline
#define LIVIO_ARK_NO_INLINE    [[gnu::noinline]]

// 分支预测优化（C++20 标准优先）
#define LIVIO_ARK_LIKELY(x)   [[likely]] (x)
#define LIVIO_ARK_UNLIKELY(x) [[unlikely]] (x)

// 标识符拼接
#define LIVIO_ARK_CONCAT_IMPL(a, b) a##b
#define LIVIO_ARK_CONCAT(a, b)      LIVIO_ARK_CONCAT_IMPL(a, b)
