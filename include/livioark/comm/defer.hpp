#pragma once
// DEFER —— 类似 Go defer 的作用域退出守卫
//
// 用法：
//   DEFER { cleanup_code(); };
//
// 在当前作用域结束（正常退出或提前 return）时，自动执行 {} 中的代码。
// 多个 DEFER 按声明的逆序执行（与 Go 行为一致）。
//
// 典型场景：事务回滚、资源释放、状态恢复

#include <utility>

namespace livio::ark::detail
{

template <typename F>
class ScopeGuard
{
public:
    explicit ScopeGuard(F&& fn) noexcept : fn_(std::move(fn)) {}
    ~ScopeGuard()
    {
        fn_();
    }

    ScopeGuard(const ScopeGuard&)            = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&)                 = delete;
    ScopeGuard& operator=(ScopeGuard&&)      = delete;

private:
    F fn_;
};

// 辅助结构，通过 operator+ 让 DEFER { ... } 语法生效
struct DeferHelper
{
    template <typename F>
    ScopeGuard<F> operator+(F&& fn) const noexcept
    {
        return ScopeGuard<F>(std::forward<F>(fn));
    }
};

}  // namespace livio::ark::detail

// 拼接宏：生成唯一变量名
#define DEFER_CAT_(a, b) a##b
#define DEFER_CAT(a, b)  DEFER_CAT_(a, b)

// DEFER { ... };
// 展开为: auto _defer_42 = livio::ark::detail::DeferHelper{} + [&]() { ... };
#define DEFER auto DEFER_CAT(_DEFER_, __LINE__) = ::livio::ark::detail::DeferHelper{} + [&]()
