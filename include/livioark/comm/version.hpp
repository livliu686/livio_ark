#pragma once

// ==============================================
// 版本号定义
// ==============================================

#ifndef LIVIO_ARK_VERSION_MAJOR
#    define LIVIO_ARK_VERSION_MAJOR 1
#endif

#ifndef LIVIO_ARK_VERSION_MINOR
#    define LIVIO_ARK_VERSION_MINOR 0
#endif

#ifndef LIVIO_ARK_VERSION_PATCH
#    define LIVIO_ARK_VERSION_PATCH 0
#endif

// 字符串化宏
#define LIVIO_ARK_STRINGIFY(x) #x

// 版本字符串，通过整型转字符串拼接
#define LIVIO_ARK_VERSION_STRING                                              \
    LIVIO_ARK_STRINGIFY(LIVIO_ARK_VERSION_MAJOR)                              \
    "." LIVIO_ARK_STRINGIFY(LIVIO_ARK_VERSION_MINOR) "." LIVIO_ARK_STRINGIFY( \
        LIVIO_ARK_VERSION_PATCH)
