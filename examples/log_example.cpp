#include "livioark/comm/version.hpp"
#include "livioark/log/log.hpp"

#include <string>

namespace log = livio::ark::log;

int main()
{
    // 可选：自定义初始化（不调用也会以默认参数自动初始化）
    log::LogOptions options;
    options.level       = log::Level::Trace;
    options.logger_name = "app";
    options.file_path   = "logs/app.log";  // 同时写入文件（滚动）
    // 在格式中加入源码位置：%s=文件名 %#=行号 %!=函数名
    options.pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [%s:%#] %v";
    log::init(options);

    LOG_INFO("LivioArk 版本 {}", LIVIO_ARK_VERSION_STRING);

    // 默认 logger —— LOG_XXX
    LOG_TRACE("trace 消息");
    LOG_DEBUG("调试值 = {}", 42);
    LOG_INFO("启动成功, 版本 {}.{}.{}", 0, 1, 0);
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

    // 查询当前级别
    LOG_INFO("当前默认级别 = {}", static_cast<int>(log::get_level()));

    // 条件日志：仅当条件为真时记录
    int retry = 5;
    LOG_WARN_IF(retry > 3, "重试次数过多: {}", retry);

    // 单次日志：循环中只记录一次
    for (int i = 0; i < 3; ++i)
    {
        LOG_INFO_ONCE("这条只会输出一次 (i={})", i);
    }

    log::flush();
    return 0;
}
