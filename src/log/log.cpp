#include "livioark/log/log.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <atomic>
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
    }

    return spdlog::level::info;
}

// 创建 sinks
std::vector<spdlog::sink_ptr> create_sinks(const LogOptions& options)
{
    std::vector<spdlog::sink_ptr> sinks;

    // 控制台 sink：Debug 模式始终添加；Release 模式由 options.console 控制
#if defined(NDEBUG)
    const bool enable_console = options.console;
#else
    const bool enable_console = true;
#endif
    if (enable_console)
    {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

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

    // 热路径缓存：Level 枚举本身按严重程度递增，可直接比较，无需加锁
    std::atomic<int>                             default_level{static_cast<int>(Level::Info)};
    std::atomic<std::shared_ptr<spdlog::logger>> default_cache;
};

static inline Registry& registry()
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
    logger->flush_on(to_spdlog_level(options.flush_level));
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
    reg.default_level.store(static_cast<int>(options.level), std::memory_order_relaxed);
    reg.default_cache.store(reg.default_logger, std::memory_order_release);
}

void set_level(Level level)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    reg.options.level = level;
    reg.default_level.store(static_cast<int>(level), std::memory_order_relaxed);
    ensure_default(reg)->set_level(to_spdlog_level(level));
}

void set_level(std::string_view mod, Level level)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    ensure_module(reg, mod)->set_level(to_spdlog_level(level));
}

Level get_level() noexcept
{
    return static_cast<Level>(registry().default_level.load(std::memory_order_relaxed));
}

Level get_level(std::string_view mod)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    auto            logger = ensure_module(reg, mod);
    return static_cast<Level>(logger->level());
}

bool should_log(Level level) noexcept
{
    // Level 枚举顺序即严重程度顺序，可与缓存阈值直接比较，避免加锁与格式化
    return static_cast<int>(level) >= registry().default_level.load(std::memory_order_relaxed);
}

bool should_log(std::string_view mod, Level level)
{
    auto&           reg = registry();
    std::lock_guard lock(reg.mutex);
    return ensure_module(reg, mod)->should_log(to_spdlog_level(level));
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

void log(Level level, std::string_view msg, const SourceLoc& loc)
{
    auto& reg = registry();
    // 热路径：先尝试无锁读取已缓存的默认 logger
    auto logger = reg.default_cache.load(std::memory_order_acquire);
    if (!logger)
    {
        std::lock_guard lock(reg.mutex);
        logger = ensure_default(reg);
        reg.default_cache.store(logger, std::memory_order_release);
    }
    logger->log(spdlog::source_loc{loc.file, loc.line, loc.func}, to_spdlog_level(level), msg);
}

void log(std::string_view mod, Level level, std::string_view msg, const SourceLoc& loc)
{
    auto&                           reg = registry();
    std::shared_ptr<spdlog::logger> logger;
    {
        std::lock_guard lock(reg.mutex);
        logger = ensure_module(reg, mod);
    }
    logger->log(spdlog::source_loc{loc.file, loc.line, loc.func}, to_spdlog_level(level), msg);
}

}  // namespace detail

}  // namespace livio::ark::log
