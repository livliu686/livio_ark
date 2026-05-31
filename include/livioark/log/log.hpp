#pragma once

#include <format>
#include <string>
#include <string_view>

#include "livioark/comm/export.hpp"

namespace livio::ark::log {

// 日志级别枚举
enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};

// 日志选项
struct LogOptions {
    Level level = Level::Info;
    std::string logger_name = "livio_ark";
    std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v";
    std::string file_path;                         // 空则不写文件
    std::size_t max_file_size = 10 * 1024 * 1024;  // 单文件最大字节
    std::size_t max_files = 3;                     // 保留文件数
    bool truncate = false;                         // 是否截断文件
};

// 初始化默认日志系统（可选；首次使用时也会以默认参数自动初始化）
LIVIO_ARK_API void init(const LogOptions& options = {});

// 设置默认日志级别
LIVIO_ARK_API void set_level(Level level);

// 设置指定模块日志级别
LIVIO_ARK_API void set_level(std::string_view mod, Level level);

// 刷新所有日志
LIVIO_ARK_API void flush();

// ------------------------------------------------------------
// 内部接收点：接收已格式化好的字符串。
// 这些函数不是模板，spdlog 完全隐藏在实现文件中。
// ------------------------------------------------------------
namespace detail {

LIVIO_ARK_API void log(Level level, std::string_view msg);
LIVIO_ARK_API void log(std::string_view mod, Level level, std::string_view msg);

} // namespace detail

// ------------------------------------------------------------
// 默认 logger 的格式化日志接口
// ------------------------------------------------------------
template <typename... Args>
void log(Level level, std::format_string<Args...> fmt, Args&&... args) {
    detail::log(level, std::format(fmt, std::forward<Args>(args)...));
}

// ------------------------------------------------------------
// 模块 logger 的格式化日志接口
// ------------------------------------------------------------
template <typename... Args>
void log_module(std::string_view mod, Level level,
                std::format_string<Args...> fmt, Args&&... args) {
    detail::log(mod, level, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace livio::ark::log

// ============================================================
// 默认 logger 日志宏
//   LOG_INFO("hello {}", name);
// ============================================================
#define LOG_TRACE(...)    ::livio::ark::log::log(::livio::ark::log::Level::Trace, __VA_ARGS__)
#define LOG_DEBUG(...)    ::livio::ark::log::log(::livio::ark::log::Level::Debug, __VA_ARGS__)
#define LOG_INFO(...)     ::livio::ark::log::log(::livio::ark::log::Level::Info, __VA_ARGS__)
#define LOG_WARN(...)     ::livio::ark::log::log(::livio::ark::log::Level::Warn, __VA_ARGS__)
#define LOG_ERROR(...)    ::livio::ark::log::log(::livio::ark::log::Level::Error, __VA_ARGS__)
#define LOG_CRITICAL(...) ::livio::ark::log::log(::livio::ark::log::Level::Critical, __VA_ARGS__)

// ============================================================
// 模块 logger 日志宏
//   LOGM_DEBUG("module A", "{} {}", 1, 2);
// ============================================================
#define LOGM_TRACE(module, ...)    ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Trace, __VA_ARGS__)
#define LOGM_DEBUG(module, ...)    ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Debug, __VA_ARGS__)
#define LOGM_INFO(module, ...)     ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Info, __VA_ARGS__)
#define LOGM_WARN(module, ...)     ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Warn, __VA_ARGS__)
#define LOGM_ERROR(module, ...)    ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Error, __VA_ARGS__)
#define LOGM_CRITICAL(module, ...) ::livio::ark::log::log_module((module), ::livio::ark::log::Level::Critical, __VA_ARGS__)
