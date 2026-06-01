#include <gtest/gtest.h>

#include "livioark/log/log.hpp"

#include <atomic>
#include <thread>

namespace ark_log = livio::ark::log;

TEST(LogTest, DefaultLoggerDoesNotThrow)
{
    ark_log::init();
    EXPECT_NO_THROW(LOG_INFO("hello {}", 42));
}

TEST(LogTest, ModuleLoggerDoesNotThrow)
{
    EXPECT_NO_THROW(LOGM_DEBUG("module A", "{} {}", 1, 2));
}

TEST(LogTest, SetLevel)
{
    EXPECT_NO_THROW(ark_log::set_level(ark_log::Level::Warn));
    EXPECT_NO_THROW(ark_log::set_level("module A", ark_log::Level::Error));
}

TEST(LogTest, GetLevelReflectsSetLevel)
{
    ark_log::set_level(ark_log::Level::Info);
    EXPECT_EQ(ark_log::get_level(), ark_log::Level::Info);

    ark_log::set_level("module B", ark_log::Level::Critical);
    EXPECT_EQ(ark_log::get_level("module B"), ark_log::Level::Critical);
    EXPECT_EQ(ark_log::get_level(), ark_log::Level::Info);
}

TEST(LogTest, ShouldLogThresholdWorks)
{
    ark_log::set_level(ark_log::Level::Warn);
    EXPECT_FALSE(ark_log::should_log(ark_log::Level::Info));
    EXPECT_TRUE(ark_log::should_log(ark_log::Level::Warn));
    EXPECT_TRUE(ark_log::should_log(ark_log::Level::Error));

    ark_log::set_level("module C", ark_log::Level::Error);
    EXPECT_FALSE(ark_log::should_log("module C", ark_log::Level::Warn));
    EXPECT_TRUE(ark_log::should_log("module C", ark_log::Level::Error));
}

TEST(LogTest, LogIfFalseDoesNotFormat)
{
    ark_log::set_level(ark_log::Level::Trace);
    std::atomic<int> eval_count{0};
    auto             side_effect = [&]()
    {
        eval_count.fetch_add(1);
        return 7;
    };

    LOG_INFO_IF(false, "value {}", side_effect());
    LOGM_WARN_IF("module D", false, "value {}", side_effect());

    EXPECT_EQ(eval_count.load(), 0);
}

TEST(LogTest, LogOnceFormatsOnlyOnce)
{
    ark_log::set_level(ark_log::Level::Trace);
    std::atomic<int> eval_count{0};
    auto             side_effect = [&]()
    {
        return eval_count.fetch_add(1) + 1;
    };

    for (int i = 0; i < 5; ++i)
    {
        LOG_INFO_ONCE("once {}", side_effect());
        LOGM_INFO_ONCE("module E", "once {}", side_effect());
    }

    EXPECT_EQ(eval_count.load(), 2);
}
