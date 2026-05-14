#pragma once

#include <string_view>
#include <utility>

// ==============================================
// 代码生成宏
// ==============================================

// 枚举转字符串自动生成
#define LIVIO_ARK_ENUM_TO_STRING(EnumType, ...) \
    inline std::string_view EnumToString(EnumType value) { \
        static const std::pair<EnumType, std::string_view> map[] = { \
            __VA_ARGS__ \
        }; \
        for (const auto& [k, v] : map) { \
            if (k == value) return v; \
        } \
        return "unknown"; \
    }

/**
// 第一步：定义你的枚举类
enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

// 第二步：使用宏自动生成转换函数
// 注意：宏必须写在枚举定义的后面！
LIVIO_ARK_ENUM_TO_STRING(LogLevel,
    {LogLevel::Debug, "DEBUG"},
    {LogLevel::Info, "INFO"},
    {LogLevel::Warn, "WARN"},
    {LogLevel::Error, "ERROR"},
    {LogLevel::Fatal, "FATAL"}
);
 */
