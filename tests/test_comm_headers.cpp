#include <gtest/gtest.h>

#include "livioark/comm/assert.hpp"
#include "livioark/comm/basic.hpp"
#include "livioark/comm/concepts.hpp"
#include "livioark/comm/defer.hpp"
#include "livioark/comm/enum_tool.hpp"
#include "livioark/comm/error_check.hpp"
#include "livioark/comm/export.hpp"
#include "livioark/comm/noncopyable.hpp"
#include "livioark/comm/performance.hpp"
#include "livioark/comm/platform.hpp"
#include "livioark/comm/singleton.hpp"

TEST(CommHeadersTest, StandaloneIncludeCompiles)
{
    SUCCEED();
}
