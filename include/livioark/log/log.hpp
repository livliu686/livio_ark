#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include "livioark/comm/export.hpp"

namespace livio::ark::log
{

// 日志级别枚举
enum class Level : uint8_t
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};

// 日志选项
struct LogOptions
{
    Level       level       = Level::Info;
    Level       flush_level = Level::Warn;
    std::string logger_name = "livio_ark";
    std::string pattern     = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v";
    std::string file_path;                         // 空则不写文件
    std::size_t max_file_size = 10 * 1024 * 1024;  // 单文件最大字节
    std::size_t max_files     = 3;                 // 保留文件数
    bool        truncate      = false;             // 是否截断文件
    // 控制台输出开关：仅在 Release 下生效；Debug 模式始终输出到控制台
    bool console = true;
};

// 初始化默认日志系统（可选；首次使用时也会以默认参数自动初始化）
LIVIO_ARK_API void init(const LogOptions& options = {});

// 设置默认日志级别
LIVIO_ARK_API void set_level(Level level);

// 设置指定模块日志级别
LIVIO_ARK_API void set_level(std::string_view mod, Level level);

// 刷新所有日志
LIVIO_ARK_API void flush();

// 查询默认 logger 当前级别
LIVIO_ARK_API Level get_level() noexcept;

// 查询指定模块 logger 当前级别
LIVIO_ARK_API Level get_level(std::string_view mod);

// 默认 logger 是否会记录该级别（用于宏内提前短路，避免无谓格式化开销）
LIVIO_ARK_API bool should_log(Level level) noexcept;

// 指定模块 logger 是否会记录该级别
LIVIO_ARK_API bool should_log(std::string_view mod, Level level);

// ------------------------------------------------------------
// 内部接收点：接收已格式化好的字符串。
// 这些函数不是模板，spdlog 完全隐藏在实现文件中。
// ------------------------------------------------------------
namespace detail
{

// 源码位置（由日志宏通过 __FILE__/__LINE__ 填充，等价于 std::source_location）
struct SourceLoc
{
    const char* file = "";
    int         line = 0;
    const char* func = "";
};

LIVIO_ARK_API void log(Level level, std::string_view msg, const SourceLoc& loc = {});
LIVIO_ARK_API void
log(std::string_view mod, Level level, std::string_view msg, const SourceLoc& loc = {});

}  // namespace detail

// ------------------------------------------------------------
// 默认 logger 的格式化日志接口
// ------------------------------------------------------------
template <typename... Args>
void log(Level level, std::format_string<Args...> fmt, Args&&... args)
{
    if (!should_log(level))
    {
        return;
    }
    detail::log(level, std::format(fmt, std::forward<Args>(args)...));
}

// 带源码位置的默认 logger 接口（供日志宏使用）
template <typename... Args>
void log_at(Level                       level,
            const detail::SourceLoc&    loc,
            std::format_string<Args...> fmt,
            Args&&... args)
{
    if (!should_log(level))
    {
        return;
    }
    detail::log(level, std::format(fmt, std::forward<Args>(args)...), loc);
}

// ------------------------------------------------------------
// 模块 logger 的格式化日志接口
// ------------------------------------------------------------
template <typename... Args>
void log_module(std::string_view mod, Level level, std::format_string<Args...> fmt, Args&&... args)
{
    if (!should_log(mod, level))
    {
        return;
    }
    detail::log(mod, level, std::format(fmt, std::forward<Args>(args)...));
}

// 带源码位置的模块 logger 接口（供日志宏使用）
template <typename... Args>
void log_module_at(std::string_view            mod,
                   Level                       level,
                   const detail::SourceLoc&    loc,
                   std::format_string<Args...> fmt,
                   Args&&... args)
{
    if (!should_log(mod, level))
    {
        return;
    }
    detail::log(mod, level, std::format(fmt, std::forward<Args>(args)...), loc);
}

}  // namespace livio::ark::log

// ============================================================
// 编译期级别裁剪
//   定义 LIVIO_LOG_ACTIVE_LEVEL 可在编译期裁剪低于该级别的日志，
//   被裁剪的日志宏不会产生任何运行时代码。
//   例：-DLIVIO_LOG_ACTIVE_LEVEL=LIVIO_LOG_LEVEL_INFO
// ============================================================
#define LIVIO_LOG_LEVEL_TRACE    0
#define LIVIO_LOG_LEVEL_DEBUG    1
#define LIVIO_LOG_LEVEL_INFO     2
#define LIVIO_LOG_LEVEL_WARN     3
#define LIVIO_LOG_LEVEL_ERROR    4
#define LIVIO_LOG_LEVEL_CRITICAL 5
#define LIVIO_LOG_LEVEL_OFF      6

#ifndef LIVIO_LOG_ACTIVE_LEVEL
#    define LIVIO_LOG_ACTIVE_LEVEL LIVIO_LOG_LEVEL_TRACE
#endif

// 源码位置宏：__func__ 在调用处的所在函数内有效
#if defined(_MSC_VER)
#    define LIVIO_ARK_LOG_FUNC __FUNCTION__
#else
#    define LIVIO_ARK_LOG_FUNC __func__
#endif

#define LIVIO_ARK_LOG_SRC_LOC                  \
    ::livio::ark::log::detail::SourceLoc       \
    {                                          \
        __FILE__, __LINE__, LIVIO_ARK_LOG_FUNC \
    }

// 内部分发宏：编译期级别裁剪 + 运行时级别短路
#define LIVIO_ARK_LOG_IMPL(LEVEL_NUM, LEVEL_ENUM, ...)                                     \
    do                                                                                     \
    {                                                                                      \
        if constexpr ((LEVEL_NUM) >= LIVIO_LOG_ACTIVE_LEVEL)                               \
        {                                                                                  \
            ::livio::ark::log::log_at(                                                     \
                ::livio::ark::log::Level::LEVEL_ENUM, LIVIO_ARK_LOG_SRC_LOC, __VA_ARGS__); \
        }                                                                                  \
    } while (0)

#define LIVIO_ARK_LOGM_IMPL(LEVEL_NUM, LEVEL_ENUM, module, ...)                    \
    do                                                                             \
    {                                                                              \
        if constexpr ((LEVEL_NUM) >= LIVIO_LOG_ACTIVE_LEVEL)                       \
        {                                                                          \
            ::livio::ark::log::log_module_at((module),                             \
                                             ::livio::ark::log::Level::LEVEL_ENUM, \
                                             LIVIO_ARK_LOG_SRC_LOC,                \
                                             __VA_ARGS__);                         \
        }                                                                          \
    } while (0)

// ============================================================
// 默认 logger 日志宏
//   LOG_INFO("hello {}", name);
// ============================================================
#define LOG_TRACE(...)    LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_TRACE, Trace, __VA_ARGS__)
#define LOG_DEBUG(...)    LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_DEBUG, Debug, __VA_ARGS__)
#define LOG_INFO(...)     LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_INFO, Info, __VA_ARGS__)
#define LOG_WARN(...)     LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_WARN, Warn, __VA_ARGS__)
#define LOG_ERROR(...)    LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_ERROR, Error, __VA_ARGS__)
#define LOG_CRITICAL(...) LIVIO_ARK_LOG_IMPL(LIVIO_LOG_LEVEL_CRITICAL, Critical, __VA_ARGS__)

// ============================================================
// 模块 logger 日志宏
//   LOGM_DEBUG("module A", "{} {}", 1, 2);
// ============================================================
#define LOGM_TRACE(module, ...) \
    LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_TRACE, Trace, module, __VA_ARGS__)
#define LOGM_DEBUG(module, ...) \
    LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_DEBUG, Debug, module, __VA_ARGS__)
#define LOGM_INFO(module, ...) LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_INFO, Info, module, __VA_ARGS__)
#define LOGM_WARN(module, ...) LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_WARN, Warn, module, __VA_ARGS__)
#define LOGM_ERROR(module, ...) \
    LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_ERROR, Error, module, __VA_ARGS__)
#define LOGM_CRITICAL(module, ...) \
    LIVIO_ARK_LOGM_IMPL(LIVIO_LOG_LEVEL_CRITICAL, Critical, module, __VA_ARGS__)

// ============================================================
// 条件日志：仅当 cond 为真时记录
//   LOG_INFO_IF(retry > 3, "retried {} times", retry);
// ============================================================
#define LOG_IF(LEVEL, cond, ...)      \
    do                                \
    {                                 \
        if (cond)                     \
        {                             \
            LOG_##LEVEL(__VA_ARGS__); \
        }                             \
    } while (0)

#define LOG_TRACE_IF(cond, ...)    LOG_IF(TRACE, cond, __VA_ARGS__)
#define LOG_DEBUG_IF(cond, ...)    LOG_IF(DEBUG, cond, __VA_ARGS__)
#define LOG_INFO_IF(cond, ...)     LOG_IF(INFO, cond, __VA_ARGS__)
#define LOG_WARN_IF(cond, ...)     LOG_IF(WARN, cond, __VA_ARGS__)
#define LOG_ERROR_IF(cond, ...)    LOG_IF(ERROR, cond, __VA_ARGS__)
#define LOG_CRITICAL_IF(cond, ...) LOG_IF(CRITICAL, cond, __VA_ARGS__)

#define LOGM_IF(LEVEL, module, cond, ...)      \
    do                                         \
    {                                          \
        if (cond)                              \
        {                                      \
            LOGM_##LEVEL(module, __VA_ARGS__); \
        }                                      \
    } while (0)

#define LOGM_TRACE_IF(module, cond, ...)    LOGM_IF(TRACE, module, cond, __VA_ARGS__)
#define LOGM_DEBUG_IF(module, cond, ...)    LOGM_IF(DEBUG, module, cond, __VA_ARGS__)
#define LOGM_INFO_IF(module, cond, ...)     LOGM_IF(INFO, module, cond, __VA_ARGS__)
#define LOGM_WARN_IF(module, cond, ...)     LOGM_IF(WARN, module, cond, __VA_ARGS__)
#define LOGM_ERROR_IF(module, cond, ...)    LOGM_IF(ERROR, module, cond, __VA_ARGS__)
#define LOGM_CRITICAL_IF(module, cond, ...) LOGM_IF(CRITICAL, module, cond, __VA_ARGS__)

// ============================================================
// 单次日志：整个程序运行期间只记录一次（线程安全，基于 magic static）
//   LOG_WARN_ONCE("deprecated API called");
// ============================================================
#define LIVIO_ARK_LOG_ONCE(expr)                                 \
    do                                                           \
    {                                                            \
        static const bool _livio_ark_logged_once = [&]() -> bool \
        {                                                        \
            expr;                                                \
            return true;                                         \
        }();                                                     \
        (void)_livio_ark_logged_once;                            \
    } while (0)

#define LOG_TRACE_ONCE(...)    LIVIO_ARK_LOG_ONCE(LOG_TRACE(__VA_ARGS__))
#define LOG_DEBUG_ONCE(...)    LIVIO_ARK_LOG_ONCE(LOG_DEBUG(__VA_ARGS__))
#define LOG_INFO_ONCE(...)     LIVIO_ARK_LOG_ONCE(LOG_INFO(__VA_ARGS__))
#define LOG_WARN_ONCE(...)     LIVIO_ARK_LOG_ONCE(LOG_WARN(__VA_ARGS__))
#define LOG_ERROR_ONCE(...)    LIVIO_ARK_LOG_ONCE(LOG_ERROR(__VA_ARGS__))
#define LOG_CRITICAL_ONCE(...) LIVIO_ARK_LOG_ONCE(LOG_CRITICAL(__VA_ARGS__))

#define LOGM_TRACE_ONCE(module, ...)    LIVIO_ARK_LOG_ONCE(LOGM_TRACE(module, __VA_ARGS__))
#define LOGM_DEBUG_ONCE(module, ...)    LIVIO_ARK_LOG_ONCE(LOGM_DEBUG(module, __VA_ARGS__))
#define LOGM_INFO_ONCE(module, ...)     LIVIO_ARK_LOG_ONCE(LOGM_INFO(module, __VA_ARGS__))
#define LOGM_WARN_ONCE(module, ...)     LIVIO_ARK_LOG_ONCE(LOGM_WARN(module, __VA_ARGS__))
#define LOGM_ERROR_ONCE(module, ...)    LIVIO_ARK_LOG_ONCE(LOGM_ERROR(module, __VA_ARGS__))
#define LOGM_CRITICAL_ONCE(module, ...) LIVIO_ARK_LOG_ONCE(LOGM_CRITICAL(module, __VA_ARGS__))
