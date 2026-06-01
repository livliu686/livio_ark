#include <gtest/gtest.h>

#include "livioark/msgbus/message_bus.h"

#include <chrono>
#include <coroutine>
#include <future>
#include <string>

namespace msgbus = livio::ark::msgbus;

template <typename T>
struct FutureTask
{
    struct promise_type
    {
        std::promise<T> promise;

        FutureTask get_return_object()
        {
            return FutureTask{promise.get_future(),
                              std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        void return_value(T value)
        {
            promise.set_value(std::move(value));
        }

        void unhandled_exception()
        {
            promise.set_exception(std::current_exception());
        }
    };

    std::future<T>                         future;
    std::coroutine_handle<promise_type> handle;

    FutureTask(std::future<T>&& f, std::coroutine_handle<promise_type> h)
        : future(std::move(f)), handle(h)
    {
    }

    FutureTask(const FutureTask&) = delete;
    FutureTask& operator=(const FutureTask&) = delete;

    FutureTask(FutureTask&& other) noexcept
        : future(std::move(other.future)), handle(other.handle)
    {
        other.handle = {};
    }

    FutureTask& operator=(FutureTask&& other) noexcept
    {
        if (this != &other)
        {
            if (handle)
            {
                handle.destroy();
            }
            future = std::move(other.future);
            handle = other.handle;
            other.handle = {};
        }
        return *this;
    }

    ~FutureTask()
    {
        if (handle)
        {
            handle.destroy();
        }
    }
};

template <typename T>
FutureTask<T> wait_one(msgbus::MessageBus& bus, std::string topic)
{
    co_return co_await bus.async_wait<T>(topic);
}

TEST(MsgBusAsyncWaitTest, ReceivesOneMessageAndCompletes)
{
    msgbus::MessageBus bus(1024, 1);
    bus.start();

    auto task = wait_one<int>(bus, "demo/async");
    EXPECT_TRUE(bus.publish<int>("demo/async", 99));

    ASSERT_EQ(task.future.wait_for(std::chrono::milliseconds(500)), std::future_status::ready);
    EXPECT_EQ(task.future.get(), 99);

    // 覆盖析构自动退订路径：未收到消息时直接销毁 awaitable。
    {
        auto cancel_task = wait_one<int>(bus, "demo/async/cancel");
        (void)cancel_task;
    }

    EXPECT_TRUE(bus.publish<int>("demo/async/cancel", 1));

    bus.stop();
}
