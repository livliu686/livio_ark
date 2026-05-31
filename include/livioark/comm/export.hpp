#pragma once
#include "platform.hpp"
// ==============================================
// 动态库符号导出导入
// ==============================================
//
// 三种状态由两个宏控制（由 CMake 注入）：
//   - LIVIO_ARK_STATIC : 库以静态库形式构建/消费（PUBLIC，会传播给消费者）
//   - LIVIO_ARK_EXPORTS: 正在构建 LivioArk 动态库本身（PRIVATE，仅库内部）
//
//   静态库（构建或消费）          -> 不加任何修饰
//   构建动态库 (LIVIO_ARK_EXPORTS) -> dllexport / visibility(default)
//   消费动态库                    -> dllimport / visibility(default)
//
#if defined(LIVIO_ARK_STATIC)
#define LIVIO_ARK_API
#elif defined(LIVIO_ARK_EXPORTS)
#define LIVIO_ARK_API LIVIO_ARK_DECL_EXPORT
#else
#define LIVIO_ARK_API LIVIO_ARK_DECL_IMPORT
#endif
