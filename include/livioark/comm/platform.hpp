#pragma once

// ==============================================
// 平台与编译器判断（最优先）
// ==============================================

// 操作系统判断
#if defined(__linux__)
#define LIVIO_ARK_OS_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
#define LIVIO_ARK_OS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#define LIVIO_ARK_OS_MACOS 1
#else
#error "Unsupported operating system"
#endif

// 编译器判断
#if defined(__GNUC__) && !defined(__clang__)
#define LIVIO_ARK_COMPILER_GCC 1
#define LIVIO_ARK_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#elif defined(__clang__)
#define LIVIO_ARK_COMPILER_CLANG 1
#define LIVIO_ARK_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
#elif defined(_MSC_VER)
#define LIVIO_ARK_COMPILER_MSVC 1
#define LIVIO_ARK_COMPILER_VERSION _MSC_VER
#else
#error "Unsupported compiler"
#endif

// C++ 版本检查（强制 C++20 及以上）
// 注意: MSVC 下 __cplusplus 默认恒为 199711L（除非 /Zc:__cplusplus），
//       需改用 _MSVC_LANG 判断实际语言标准。
#if defined(_MSVC_LANG)
#define LIVIO_ARK_CPLUSPLUS _MSVC_LANG
#else
#define LIVIO_ARK_CPLUSPLUS __cplusplus
#endif

#if LIVIO_ARK_CPLUSPLUS < 202002L
#error "LivioArk requires C++20 or later. Please enable C++20 in your compiler."
#endif
#define LIVIO_ARK_CPP20 1


// 公共导出导入宏（类似Qt的Q_DECL_EXPORT/Q_DECL_IMPORT）
#if defined(LIVIO_ARK_OS_WINDOWS)
#define LIVIO_ARK_DECL_EXPORT __declspec(dllexport)
#define LIVIO_ARK_DECL_IMPORT __declspec(dllimport)
#else
#define LIVIO_ARK_DECL_EXPORT __attribute__((visibility("default")))
#define LIVIO_ARK_DECL_IMPORT __attribute__((visibility("default")))
#endif
