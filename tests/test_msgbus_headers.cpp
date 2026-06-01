#include <gtest/gtest.h>

#include "livioark/msgbus/config.h"
#include "livioark/msgbus/lock_free_queue.h"
#include "livioark/msgbus/message.h"
#include "livioark/msgbus/message_bus.h"
#include "livioark/msgbus/object_pool.h"
#include "livioark/msgbus/subscriber.h"
#include "livioark/msgbus/topic_matcher.h"
#include "livioark/msgbus/topic_registry.h"
#include "livioark/msgbus/topic_slot.h"
#include "livioark/msgbus/wildcard_trie.h"

TEST(MsgBusHeadersTest, StandaloneIncludeCompiles)
{
    SUCCEED();
}
