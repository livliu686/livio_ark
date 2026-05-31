#include "livioark/log/log.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace livio::ark::log
{

namespace
{

// 转换 Level 到 spdlog::level
spdlog::level::level_enum to_spdlog_level(Level level)
{
    switch (level)
    {
        case Level::Trace:
            return spdlog::level::trace;
        case Level::Debug:
            return spdlog::level::debug;
        case Level::Info:
            return spdlog::level::info;
        case Level::Warn:
            return spdlog::level::warn;
        case Level::Error:
            return spdlog::level::err;
        case Level::Critical:
            return spdlog::level::critical;
        case Level::Off:
            return spdlog::level::off;
        default:
            return spdlog::level::info;
    }
}

// 创建 sinks
std::vector<spdlog::sink_ptr> create_sinks(const LogOptions& options)
{
    std::vector<spdlog::sink_ptr> sinks;

    // 控制台 sink
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    // 文件 sink（如果指定）
    if (!options.file_path.empty())
    {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            options.file_path, options.max_file_size, options.max_files, options.truncate));
    }

    return sinks;
}

// 全局状态：默认 logger、配置与模块 logger 注册表
struct Registry
{
    std::mutex                                                       mutex;
    LogOptions                                                       options;
    std::shared_ptr<spdlog::logger>                                  default_logger;
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> modules;
};

Registry& registry()
{
    static Registry instance;
    return instance;
}

std::shared_ptr<spdlog::logger> make_logger(const std::string& name, const LogOptions& options)
{
    auto sinks  = create_sinks(options);
    auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    logger->set_level(to_spdlog_level(options.level));
    logger->set_pattern(options.pattern);
    logger->flush_on(spdlog::level::warn);
    return logger;
}

// 获取默认 logger（按需以默认参数初始化）
std::shared_ptr<spdlog::logger> ensure_default(Registry& reg)
{
    if (!reg.default_logger)
    {
        reg.default_logger = make_logger(reg.options.logger_name, reg.options);
    }
    return reg.default_logger;
}

// 获取或创建模块 logger
std::shared_ptr<spdlog::logger> ensure_module(Registry& reg, std::string_view mod)
{
    std::string key(mod);
    if (auto it = reg.modules.find(key); it != reg.modules.end())
    {
        return it->second;
    }
    LogOptions opts  = reg.options;
    opts.logger_name = key;
    auto logger      = make_logger(key, opts);
    reg.modules.emplace(key, logger);
    return logger;
}

}  // anonymous namespace

void init(const LogOptions& options)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    reg.options        = options;
    reg.default_logger = make_logger(options.logger_name, options);
}

void set_level(Level level)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    reg.options.level = level;
    ensure_default(reg)->set_level(to_spdlog_level(level));
}

void set_level(std::string_view mod, Level level)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    ensure_module(reg, mod)->set_level(to_spdlog_level(level));
}

void flush()
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    if (reg.default_logger)
        reg.default_logger->flush();
    for (auto& [name, logger] : reg.modules)
    {
        logger->flush();
    }
}

namespace detail
{

void log(Level level, std::string_view msg)
{
    auto&                           reg = registry();
    std::shared_ptr<spdlog::logger> logger;
    {
        std::lock_guard lock(reg.mutex);
        logger = ensure_default(reg);
    }
    logger->log(to_spdlog_level(level), msg);
}

void log(std::string_view mod, Level level, std::string_view msg)
{
    auto&                           reg = registry();
    std::shared_ptr<spdlog::logger> logger;
    {
        std::lock_guard lock(reg.mutex);
        logger = ensure_module(reg, mod);
    }
    logger->log(to_spdlog_level(level), msg);
}

}  // namespace detail

}  // namespace livio::ark::log
