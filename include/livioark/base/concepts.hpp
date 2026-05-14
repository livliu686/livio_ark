#pragma once

#include "basic.hpp"
#include <functional>
#include <type_traits>

// ==============================================
// 核心基础工具类
// ==============================================

LIVIO_ARK_NAMESPACE_BEGIN

// 概念约束（C++20）
template<typename T>
concept Copyable = std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;

template<typename T>
concept Movable = std::is_move_constructible_v<T> && std::is_move_assignable_v<T>;

template<typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

LIVIO_ARK_NAMESPACE_END
