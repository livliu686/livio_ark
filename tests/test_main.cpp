#include <gtest/gtest.h>

#include "livioark/log/log.hpp"

namespace log = livio::ark::log;

TEST(LogTest, DefaultLoggerDoesNotThrow) {
    log::init();
    EXPECT_NO_THROW(LOG_INFO("hello {}", 42));
}

TEST(LogTest, ModuleLoggerDoesNotThrow) {
    EXPECT_NO_THROW(LOGM_DEBUG("module A", "{} {}", 1, 2));
}

TEST(LogTest, SetLevel) {
    EXPECT_NO_THROW(log::set_level(log::Level::Warn));
    EXPECT_NO_THROW(log::set_level("module A", log::Level::Error));
}
