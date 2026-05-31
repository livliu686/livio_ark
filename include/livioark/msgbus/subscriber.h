#pragma once

#include <cstdint>
#include <functional>

namespace livio::ark::msgbus
{

using SubscriptionId = uint64_t;

template <typename T>
struct Subscriber
{
    SubscriptionId                id;
    std::function<void(const T&)> handler;
};

}  // namespace livio::ark::msgbus
