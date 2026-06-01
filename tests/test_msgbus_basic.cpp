#include <gtest/gtest.h>

#include "livioark/msgbus/message_bus.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace msgbus = livio::ark::msgbus;

TEST(MsgBusTest, PublishSubscribeBasic)
{
    msgbus::MessageBus bus(1024, 1);
    std::atomic<int>   received{-1};

    bus.start();
    auto sub_id = bus.subscribe<int>("demo/basic", [&](const int& v) { received.store(v); });

    EXPECT_NE(sub_id, 0U);
    EXPECT_TRUE(bus.publish<int>("demo/basic", 42));

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while (received.load() != 42 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    EXPECT_EQ(received.load(), 42);
    bus.stop();
}

TEST(MsgBusTest, WildcardSubscription)
{
    msgbus::MessageBus bus(1024, 1);
    std::atomic<int>   hits{0};

    bus.start();
    auto sub_id = bus.subscribe<int>("demo/#", [&](const int&) { hits.fetch_add(1); });

    EXPECT_NE(sub_id, 0U);
    EXPECT_TRUE(bus.publish<int>("demo/a", 1));
    EXPECT_TRUE(bus.publish<int>("demo/a/b", 2));

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while (hits.load() < 2 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    EXPECT_EQ(hits.load(), 2);
    bus.stop();
}

TEST(MsgBusTest, TopicTypeMismatchThrows)
{
    msgbus::MessageBus bus;
    bus.start();

    auto sub_id = bus.subscribe<int>("demo/type", [](const int&) {});
    EXPECT_NE(sub_id, 0U);

    EXPECT_THROW((bus.subscribe<std::string>("demo/type", [](const std::string&) {})), std::runtime_error);

    bus.stop();
}

TEST(MsgBusTest, SameTopicOrderPreservedWithMultiDispatcher)
{
    msgbus::MessageBus  bus(4096, 4);
    std::vector<int>    received;
    std::mutex          mu;
    std::condition_variable cv;

    bus.start();
    bus.subscribe<int>("demo/order", [&](const int& v)
    {
        {
            std::lock_guard<std::mutex> lock(mu);
            received.push_back(v);
        }
        cv.notify_one();
    });

    constexpr int kCount = 200;
    for (int i = 0; i < kCount; ++i)
    {
        EXPECT_TRUE(bus.publish<int>("demo/order", i));
    }

    {
        std::unique_lock<std::mutex> lock(mu);
        cv.wait_until(lock,
                      std::chrono::steady_clock::now() + std::chrono::seconds(2),
                      [&] { return static_cast<int>(received.size()) == kCount; });
        ASSERT_EQ(static_cast<int>(received.size()), kCount);
        for (int i = 0; i < kCount; ++i)
        {
            EXPECT_EQ(received[i], i);
        }
    }

    bus.stop();
}

TEST(MsgBusTest, BackpressurePolicies)
{
    {
        msgbus::MessageBus bus(1, 1, msgbus::FullPolicy::ReturnFalse);
        EXPECT_TRUE(bus.publish<int>("demo/policy/return_false", 1));
        EXPECT_TRUE(bus.publish<int>("demo/policy/return_false", 2));
        EXPECT_FALSE(bus.publish<int>("demo/policy/return_false", 3));
    }

    {
        msgbus::MessageBus bus(1, 1, msgbus::FullPolicy::DropNewest);
        EXPECT_TRUE(bus.publish<int>("demo/policy/drop_newest", 1));
        EXPECT_TRUE(bus.publish<int>("demo/policy/drop_newest", 2));
    }

    {
        msgbus::MessageBus bus(1, 1, msgbus::FullPolicy::DropOldest);
        EXPECT_TRUE(bus.publish<int>("demo/policy/drop_oldest", 1));
        EXPECT_TRUE(bus.publish<int>("demo/policy/drop_oldest", 2));
    }

    {
        msgbus::MessageBus bus(1,
                               1,
                               msgbus::FullPolicy::BlockTimeout,
                               std::chrono::milliseconds(20));
        EXPECT_TRUE(bus.publish<int>("demo/policy/block_timeout", 1));
        EXPECT_TRUE(bus.publish<int>("demo/policy/block_timeout", 2));
        EXPECT_FALSE(bus.publish<int>("demo/policy/block_timeout", 3));
    }
}

TEST(MsgBusTest, UnsubscribeInsideHandler)
{
    msgbus::MessageBus bus(1024, 1);
    std::atomic<int>   calls{0};
    msgbus::SubscriptionId sub_id = 0;

    bus.start();
    sub_id = bus.subscribe<int>("demo/unsub", [&](const int&)
    {
        calls.fetch_add(1);
        bus.unsubscribe(sub_id);
    });

    EXPECT_TRUE(bus.publish<int>("demo/unsub", 1));

    const auto first_deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while (calls.load() < 1 && std::chrono::steady_clock::now() < first_deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    ASSERT_EQ(calls.load(), 1);

    EXPECT_TRUE(bus.publish<int>("demo/unsub", 2));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(calls.load(), 1);

    bus.stop();
}
