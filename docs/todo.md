# LivioArk 待办列表

> 来源：工程 review（2026-05-31，已二次更新）。按阶段推进，每阶段可独立提交。

## 阶段 1 — 一致性收口（低风险、高收益）
- [x] 统一命名空间为 `livio::ark::<module>`
  - [x] `livio_ark::comm` → `livio::ark::comm`（enum_tool.hpp）
  - [x] `msgbus` → `livio::ark::msgbus`
  - [x] 核对 `livio::ark::base` / `livio::ark::detail` 是否保留（保留为子命名空间）
- [x] 修正 defer.hpp 结尾残留注释 `// namespace nova::detail`
- [x] 版本号单一真相源：CMake `configure_file` 注入 version.hpp（project VERSION=0.1.0）

## 阶段 2 — 日志库增强
- [x] 模板内先 `should_log(level)` 再 `std::format`，避免无谓格式化开销
- [x] 默认 logger 缓存，减少每条日志的锁竞争（atomic 级别 + atomic shared_ptr 缓存）
- [x] 增加 `get_level()`（默认与模块）
- [x] 支持源码位置（`SourceLoc`，由宏注入 `__FILE__/__LINE__/__func__`，pattern 用 `%s:%#:%!`）
- [x] 增加 `LOG_*_IF` / `LOG_*_ONCE` 宏（默认与模块）
- [x] 编译期级别裁剪宏（`LIVIO_LOG_ACTIVE_LEVEL`）
- [x] `LogOptions` 增加 `console` 开关（Release 生效，Debug 始终输出控制台）

## 阶段 3 — 头文件自给自足性（高优先级，低风险）
> review 发现：多个 comm 头单独包含会编译失败，缺少自身依赖的 include。
- [ ] `error_check.hpp` 补 `#include "basic.hpp"`、`<source_location>`、`<cstdio>`、`<cstdlib>`
      （用到 `LIVIO_ARK_UNLIKELY` / `std::source_location` / `fprintf` / `std::abort`）
- [ ] `performance.hpp` 补 `#include "platform.hpp"`（用到 `LIVIO_ARK_COMPILER_GCC/CLANG`）
- [ ] 全量核对其余 comm 头的 include 自给自足性，确保任意单文件可独立编译
- [ ] 加一个“逐头包含”编译测试，防止回归

## 阶段 4 — 构建工程化
- [ ] `install(TARGETS)` + 生成 `LivioArkConfig.cmake`，支持下游 `find_package(LivioArk)`
      （需一并安装 `include/` 与生成的 `version.hpp`）
- [ ] 添加 `CMakePresets.json`（debug/release × shared/static）
- [ ] 接入 GoogleTest（thirdparty 或 FetchContent），默认打开测试并实际跑通
- [x] 确认 `build/`、`cmake-build-debug/` 等 IDE 产物未被 git 跟踪（已确认 0 个被跟踪）

## 阶段 5 — 模块补全
- [ ] `msgbus` 纳入编译并补单元测试（10 个头文件，约 900 行模板代码，当前零编译验证）
      - [ ] 先加纯包含编译目标确保可编译，再补单测
- [ ] `concurrent`：实现或移除空的 `thread_pool.cpp`，补头文件
- [ ] 补全或移除空示例：`defer_example.cpp`、`thread_pool_example.cpp`

## 阶段 6 — 代码质量打磨（低优先级）
- [ ] `to_spdlog_level` 移除不可达的 `default` 分支，恢复 `-Wswitch` 穷尽性检查
- [ ] `LIVIO_ARK_FORCE_INLINE` 跨编译器实现（MSVC `__forceinline` / GCC·Clang `always_inline`）
- [ ] `singleton.hpp` 宏版与模板版重叠，统一保留模板 `base::Singleton`
- [ ] `flush_on(warn)` 阈值硬编码，考虑提升为 `LogOptions` 配置项

## 阶段 7 — 文档 / CI
- [ ] 编写 README（用法、构建、API；当前为空）
- [ ] GitHub Actions：Windows + Linux，shared/static 构建矩阵

## 阶段 8 — UT 补充计划（本次 review 新增）
- [ ] [UT][log] `get_level()/set_level()` 行为断言：设置后可稳定读回，模块级与默认级互不影响
- [ ] [UT][log] `should_log()` 阈值断言：不同级别下返回值正确（含模块 logger）
- [ ] [UT][log] `LOG_*_ONCE` / `LOGM_*_ONCE` 只触发一次（并发场景下仍只记录一次）
- [ ] [UT][log] `LOG_*_IF` / `LOGM_*_IF` 条件为假时不触发格式化路径（可用副作用计数器断言）
- [ ] [UT][log] 编译期裁剪回归：`LIVIO_LOG_ACTIVE_LEVEL` 生效（低级别日志宏被裁剪）
- [ ] [UT][comm] 逐头包含编译测试：`include/livioark/comm/*.hpp` 任意单头可独立编译
- [ ] [UT][msgbus] 先把 `msgbus` 纳入测试编译目标，确保 10 个头至少有编译级验证
- [ ] [UT][msgbus] 单线程分发：publish/subscribe 基本收发与 topic type mismatch 异常路径
- [ ] [UT][msgbus] 多分发线程：同 topic 保序、不同 topic 并发分发
- [ ] [UT][msgbus] 背压策略覆盖：ReturnFalse/DropOldest/DropNewest/Block/BlockTimeout
- [ ] [UT][msgbus] 通配符订阅：`*` 与 `#` 匹配规则及非法 pattern 校验
- [ ] [UT][msgbus] `async_wait` 协程路径：只触发一次、析构自动退订、超时/停机边界
- [ ] [UT][msgbus] handler 内 `unsubscribe` 重入安全，确保当前回调完成且后续不再触发
- [ ] [UT][build] CMake 测试链路回归：本地 thirdparty / find_package / FetchContent 三条路径至少各验证一次
