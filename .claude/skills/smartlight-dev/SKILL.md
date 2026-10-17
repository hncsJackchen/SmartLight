---
name: smartlight-dev
description: 智能家居开灯项目的固件开发与联调助手。当用户要改 ESP-12F 主控固件、改 ASRPRO 语音固件、同步 UART 协议、调舵机/传感器阈值，或改动需要同步 CLAUDE/README/软件设计/需求文档时使用。触发词如「改协议」「调舵机」「改阈值」「改主控/语音固件」「联调」。
---

# SmartLight 固件开发助手

面向本仓库（`SmartLight`，ESP-12F 主控）与语音固件仓库（`../SmartLightAsrPro`，ASRPRO），双 MCU 协作。

## 项目地图

- 主控固件 `src/`（ESP-12F，PlatformIO + Arduino）
  - `config.h` — 集中配置：引脚、阈值、舵机角度、协议宏、`DEBUG_LOG`
  - `main.cpp` — 初始化 + 主循环；`AppLight.cpp` — 主逻辑；`ServoSwitch.cpp` — 舵机；`Sensors.cpp` — BH1750 / HC-SR04；`Voice.cpp` — UART 行协议
- 语音固件 `../SmartLightAsrPro/SmartLight-ASRPRO.hd` — 天问Block 单文件项目（无本地构建）
- 协议契约：`doc/软件设计.md` 第 6 节；需求：`doc/智能家居+AI开灯 (2).md`

## 工作准则

- **改完必验证**：主控固件用 `C:/Users/Admin/.platformio/penv/Scripts/platformio.exe run` 编译；语音固件无法本地编译，要明确提醒用户去天问IDE 编译烧录验证。
- **协议必须两端同步**：改 UART 协议要同时改 `config.h` 的宏、`.hd` 的收发分支、`doc/软件设计.md` 协议表，三处一起改。
- **改需求/设计同步文档**：`CLAUDE.md`、`README.md`、`doc/软件设计.md`、`doc/智能家居+AI开灯 (2).md`（本仓库约定）。
- **舵机角度是绝对角度**：`servo.write()` 写 0~180 绝对角，不是旋转量；复位中位是 90°。
- 提交走 Conventional Commits 中文格式，末尾带 `Co-Authored-By: Claude Code <noreply@anthropic.com>`。

## 关键常量速查

| 项 | 值 |
|---|---|
| 光照阈值 | `LUX_THRESHOLD = 1` lx |
| 进门距离阈值 | `DIST_THRESHOLD_CM = 20` cm |
| 舵机 | 开灯 180°、关灯 0°、复位 90° |
| UART | 9600 8N1，文本行帧，`\n` 结尾 |
| 协议帧 | `ON`/`OFF` → `ON_OK`/`OFF_OK`/`ERROR` |
| 命令词 ID | 1 打开灯光 / 2 关闭灯光 / 3 开灯 / 4 关灯 / 5 退出 |

## 自动开灯逻辑（触发标记）

`cond = (dist < 20 && lux < 1)`，`trigFlag` 检测其上升沿：`cond && !trigFlag` → 置有效并开灯；`!cond` → 复位。持续满足不重复触发。