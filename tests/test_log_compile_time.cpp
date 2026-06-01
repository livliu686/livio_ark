#include <gtest/gtest.h>

#include "livioark/log/log.hpp"

int trace_level_only_symbol();

TEST(LogCompileTimeTest, TraceIsCompiledOutAtInfoLevel)
{
    // If LIVIO_LOG_ACTIVE_LEVEL removes TRACE at compile-time, this symbol is never ODR-used.
    LOG_TRACE("trace {}", trace_level_only_symbol());
    SUCCEED();
}
