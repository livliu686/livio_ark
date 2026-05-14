#pragma once

// ==============================================
// 性能优化宏
// ==============================================

// CPU 缓存行大小（x86/ARM 通用）
#define LIVIO_ARK_CACHE_LINE_SIZE 64

// 对齐控制
#define LIVIO_ARK_ALIGNAS(x) alignas(x)
#define LIVIO_ARK_CACHE_ALIGNED LIVIO_ARK_ALIGNAS(LIVIO_ARK_CACHE_LINE_SIZE)

// 空基类优化
#define LIVIO_ARK_NO_UNIQUE_ADDRESS [[no_unique_address]]

// 内存预取
#if defined(LIVIO_ARK_COMPILER_GCC) || defined(LIVIO_ARK_COMPILER_CLANG)
#define LIVIO_ARK_PREFETCH(addr) __builtin_prefetch(addr)
#define LIVIO_ARK_PREFETCH_L2(addr) __builtin_prefetch(addr, 0, 1)
#else
#define LIVIO_ARK_PREFETCH(addr) ((void)0)
#define LIVIO_ARK_PREFETCH_L2(addr) ((void)0)
#endif

// 位标志操作
#define LIVIO_ARK_BIT(x) (1ULL << (x))
#define LIVIO_ARK_HAS_BIT(flags, bit) ((flags & (bit)) != 0)
#define LIVIO_ARK_SET_BIT(flags, bit) ((flags) |= (bit))
#define LIVIO_ARK_CLEAR_BIT(flags, bit) ((flags) &= ~(bit))
