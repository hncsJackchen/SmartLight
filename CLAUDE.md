# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

# 项目概览

智能家居 AI 开灯产品：进门自动感应开灯 + 语音开关灯。

- 自动开灯：当环境光照 < 1 lx 且（超声波测距 < 20 cm 或 PIR 检测到人）时，舵机从中位 90° 顺时针转 90° 打开灯、再回正中位；条件不同时满足则不动作。
- 语音控制：ASRPRO 识别“打开灯光/关闭灯光”后经 UART 发指令给 ESP-12F，ESP-12F 执行舵机动作并把结果回传，ASRPRO 用喇叭播报回复语。

# 当前状态

仓库处于开发早期。固件开发重点是 **ESP-12F**（本仓库实现）；**ASRPRO 语音模块固件在另一个项目中实现**，本仓库只负责与它约定串口协议并对接，不开发其固件。

工具链：
- ESP-12F：VS Code + PlatformIO（platform `espressif8266`，board `esp12e`，framework `arduino`）。
  - 构建 `pio run`；烧录 `pio run -t upload`；串口监视 `pio device monitor`。
- ASRPRO：唯创知音天问Block / 天问IDE（闭源私有工具链，不支持 PlatformIO），不在本仓库。

需求来源：[doc/智能家居+AI开灯 (2).md](doc/智能家居+AI开灯 (2).md)；软件设计见 [doc/软件设计.md](doc/软件设计.md)。改需求/设计需同步对应文档。

# 技术选型

|类型|子类型|型号|是否使用|
|---|---|---|---|
|硬件|核心板|ESP-12F|使用|
|硬件|舵机|SG90|使用|
|硬件|光传感器|GY-302 / BH1750|使用|
|硬件|声波测距|HC-SR04|使用|
|硬件|人体红外|PIR / HC-SR501|使用|
|硬件|语音模块|ASRPRO|使用|
|硬件|拾音/放音|mic / speaker（与 ASRPRO 配套）|使用|
|软件|控制固件|ESP-12F（本仓库）|使用|
|软件|语音固件|ASRPRO（另一个项目）|使用|
|软件|工具链|VS Code + PlatformIO|使用|

# 架构要点

- 双 MCU 协作：ESP-12F 为主控（决策 + 采集 + 驱动），ASRPRO 为语音外设。二者 **UART 双向**通信：ASRPRO→ESP-12F 发开/关灯指令，ESP-12F→ASRPRO 回执行结果供喇叭播报。
- 执行器：SG90 舵机双向拨动——开灯从中位 90° 顺时针转 90°（到 180°）、关灯逆时针转 90°（到 0°），拨后都回正中位 90°。
- 传感器：BH1750（I2C 光照）、HC-SR04（超声波进门检测）、PIR（人体红外，与超声波并联）。
- 控制规则：自动开灯的触发条件为「光照<1 lx 且（距离<20 cm 或 PIR 有人）」，用触发标记检测其上升沿（条件满足且标记无效→置有效并开灯，条件解除→复位）；语音开/关灯每次都执行拨动并回传结果（无状态缓存、无幂等）。
- 关键点：固件不维护灯状态，每次语音指令 / 自动触发都直接执行对应动作（开灯顺转 90° 到 180°、关灯逆转 90° 到 0°，拨后回中位 90°）。
- UART 协议（与 ASRPRO 项目的接口契约）见 [doc/软件设计.md](doc/软件设计.md)。