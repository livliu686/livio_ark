#include <gtest/gtest.h>

#include "livioark/log/log.hpp"

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
