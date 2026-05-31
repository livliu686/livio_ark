# LivioArk 待办列表

> 来源：工程 review（2026-05-31）。按阶段推进，每阶段可独立提交。

## 阶段 1 — 一致性收口（低风险、高收益）
- [ ] 统一命名空间为 `livio::ark::<module>`
  - [ ] `livio_ark::comm` → `livio::ark::comm`（enum_tool.hpp）
  - [ ] `msgbus` → `livio::ark::msgbus`
  - [ ] 核对 `livio::ark::base` / `livio::ark::detail` 是否保留
- [ ] 修正 defer.hpp 结尾残留注释 `// namespace nova::detail`
- [ ] 版本号单一真相源：CMake `configure_file` 注入 version.hpp（当前 version.hpp=1.0.0 与 project VERSION=0.0.0.1 不一致）

## 阶段 2 — 日志库增强
- [ ] 模板内先 `should_log(level)` 再 `std::format`，避免无谓格式化开销
- [ ] 默认 logger 缓存，减少每条日志的锁竞争（atomic/一次性缓存）
- [ ] 增加 `get_level()`
- [ ] 支持 `std::source_location`（文件/行号）
- [ ] 增加 `LOG_*_IF` / `LOG_*_ONCE` 宏
- [ ] 编译期级别裁剪宏（类似 `SPDLOG_ACTIVE_LEVEL` 的 `LIVIO_LOG_LEVEL`）

## 阶段 3 — 构建工程化
- [ ] `install(TARGETS)` + 生成 `LivioArkConfig.cmake`，支持下游 `find_package(LivioArk)`
- [ ] 添加 `CMakePresets.json`（debug/release × shared/static）
- [ ] 接入 GoogleTest（thirdparty 或 FetchContent），默认打开测试
- [ ] 确认 `build/`、`cmake-build-debug/` 等 IDE 产物未被 git 跟踪

## 阶段 4 — 模块补全
- [ ] `msgbus` 纳入编译并补单元测试（当前仅头文件，未验证可编译）
- [ ] `concurrent`：实现或移除空的 `thread_pool.cpp`，补头文件
- [ ] 补全或移除空示例：`defer_example.cpp`、`thread_pool_example.cpp`

## 阶段 5 — 文档 / CI
- [ ] 编写 README（用法、构建、API）
- [ ] GitHub Actions：Windows + Linux，shared/static 构建矩阵
