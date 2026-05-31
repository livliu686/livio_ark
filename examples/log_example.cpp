#include "livioark/log/log.hpp"

#include <string>

namespace log = livio::ark::log;

int main() {
    // 可选：自定义初始化（不调用也会以默认参数自动初始化）
    log::LogOptions options;
    options.level = log::Level::Trace;
    options.logger_name = "app";
    options.file_path = "logs/app.log";  // 同时写入文件（滚动）
    log::init(options);

    // 默认 logger —— LOG_XXX
    LOG_TRACE("trace 消息");
    LOG_DEBUG("调试值 = {}", 42);
    LOG_INFO("启动成功, 版本 {}.{}.{}", 0, 0, 1);
    LOG_WARN("内存使用率 {}%", 87.5);
    LOG_ERROR("打开文件失败: {}", std::string{"config.json"});
    LOG_CRITICAL("致命错误码 {:#x}", 0xDEAD);

    // 模块 logger —— LOGM_XXX
    LOGM_DEBUG("module A", "{} {}", 1, 2);
    LOGM_INFO("network", "连接到 {}:{}", "127.0.0.1", 8080);
    LOGM_ERROR("db", "查询超时, 耗时 {} ms", 1500);

    // 单独调整某个模块的级别
    log::set_level("module A", log::Level::Warn);
    LOGM_DEBUG("module A", "这条不会输出");
    LOGM_WARN("module A", "这条会输出");

    log::flush();
    return 0;
}
