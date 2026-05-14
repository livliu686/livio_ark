#pragma once
#include "platform.hpp"
// ==============================================
// 动态库符号导出导入
// ==============================================

// 便捷API宏（基于构建配置自动选择）
#if defined(LIVIO_ARK_BUILD_SHARED)
#define LIVIO_ARK_API LIVIO_ARK_DECL_EXPORT
#else
#define LIVIO_ARK_API LIVIO_ARK_DECL_IMPORT
#endif
