# 智能家居 AI 开灯

基于 **ESP8266**（ESP-12F / NodeMCU）的 Arduino 固件，通过 PlatformIO 开发。采集环境光照与超声波距离，实现进门自动开灯（光照 < 1 lx 且距离 < 20 cm）；经 UART 对接 **ASRPRO 语音模块**实现语音开关灯，用 **SG90 舵机**拨动实体灯光开关并回传结果供喇叭播报。

## 目录

- [产品介绍](#产品介绍)
- [1. 开发板引脚说明](#1-开发板引脚说明)
- [2. 外设说明表](#2-外设说明表)
  - [2.1 GY-302 光照传感器](#21-gy-302-光照传感器)
  - [2.2 HC-SR04 超声波测距](#22-hc-sr04-超声波测距)
  - [2.3 SG90 舵机](#23-sg90-舵机)
  - [2.4 ASRPRO 语音模块](#24-asrpro-语音模块)
- [3. 代码介绍](#3-代码介绍)
  - [3.1 目录结构](#31-目录结构)
  - [3.2 依赖库](#32-依赖库)
  - [3.3 构建与烧录](#33-构建与烧录)
- [4. 通信：与 ASRPRO 的 UART 协议](#4-通信与-asrpro-的-uart-协议)
- [5. 其它](#5-其它)

## 产品介绍

一款装在家门口的智能开灯装置：检测到有人进门、且当时环境昏暗时自动开灯，也支持语音开关灯，全程无需手动摸开关。

**使用场景**

- 玄关 / 门厅 / 走廊入口等进出频繁、光线不足的区域。
- 双手提物腾不出手开灯的场合（拎菜、搬快递回家等）：进门即自动亮灯，无需摸开关。

**触发场景**

| 场景 | 触发条件 | 动作 |
|---|---|---|
| 自动开灯 | 光照 < 1 lx **且** 检测到进门（距离 < 20 cm） | 舵机拨开关开灯 |
| 语音开灯 | 说「打开灯光」，ASRPRO 识别后经 UART 发指令 | 舵机拨开关开灯 |
| 语音关灯 | 说「关闭灯光」，ASRPRO 识别后经 UART 发指令 | 舵机拨开关关灯 |

> 语音与自动共用同一套拨开关动作；固件不缓存灯状态，每次指令/触发都直接拨动一次。

## 1. 开发板引脚说明

本项目实际使用 **LoLin NodeMCU v3**（丝印含 D0~D10，D9/D10 即 RX/TX）。排针按实物物理顺序（左/右两排）如下：

> 标记：
> ✅ = 本项目占用
> ⚪ = 空闲可用
> 🟡 = 启动脚 / SPI Flash（有电平约束或被 Flash 占用，勿接外设）

| 左侧引脚 | 标注 | 右侧引脚 | 标注 |
|---|---|---|---|
| A0 | ⚪ ADC 输入 | D0 | ⚪ GPIO16（无中断） |
| GND | 地 | D1 | ✅ BH1750 SCL（GPIO5） |
| VU | 5V 输出（USB 5V） | D2 | ✅ BH1750 SDA（GPIO4） |
| SD3 | 🟡 Flash | D3 | 🟡 GPIO0 启动脚 |
| SD2 | 🟡 Flash | D4 | ✅ 板载 LED（GPIO2，常亮指示） |
| SD1 | 🟡 Flash MOSI | 3V3 | 电源 3.3V |
| CMD | 🟡 Flash CS | GND | 地 |
| SD0 | 🟡 Flash MISO | D5 | ✅ SG90 信号（GPIO14） |
| CLK | 🟡 Flash SCK | D6 | ✅ HC-SR04 Echo（GPIO12） |
| GND | 地 | D7 | ✅ HC-SR04 Trig（GPIO13） |
| 3V3 | 电源 3.3V | D8 | 🟡 GPIO15 启动脚 |
| EN | 使能（CH_PD） | D9 | ✅ ASRPRO→ESP RX（GPIO3） |
| RST | 复位 | D10 | ✅ ESP→ASRPRO TX（GPIO1） |
| GND | 地 | GND | 地 |
| Vin | 5V 输入 | 3V3 | 电源 3.3V |

> SD0~SD3、CMD、CLK 即 GPIO6~11，被 SPI Flash 占用，不可接外设。D3(GPIO0)/D4(GPIO2)/D8(GPIO15) 为启动脚，务必保持上电默认电平（D3/D4 高，D8 低）。D4 同时是板载 LED（低电平点亮），固件在 setup 拉低使其常亮作为运行指示（boot 已完成，不影响 GPIO2 启动）。5V 取电可用 VU（USB 供电时输出 5V）或 Vin 接入外部电源。

## 2. 外设说明表

### 2.1 GY-302 光照传感器

连续高分辨率模式（1 lx，120ms），I2C 地址自动探测（0x23 / 0x5C）。

| 外设引脚 | ESP8266 引脚 | 备注 |
|---|---|---|
| SDA | GPIO4 (D2) | — |
| SCL | GPIO5 (D1) | — |
| VCC | 3.3V | — |
| GND | GND | — |
| ADDR | GND | 接地 → 地址 0x23（悬空/接 VCC 得 0x5C，固件自动探测） |

### 2.2 HC-SR04 超声波测距

每 100ms 测距一次（30ms 超时 ≈ 5m）。用于进门检测。

| 外设引脚 | ESP8266 引脚 | 备注 |
|---|---|---|
| Trig | GPIO13 (D7) | — |
| Echo | GPIO12 (D6) | 3.3V 供电下 Echo 约 3.3V，直连 |
| VCC | 3.3V | — |
| GND | GND | — |

### 2.3 SG90 舵机

「开灯拨到 180°、关灯拨到 0°，拨完复位回中位 90°」拨动开关。固件不缓存灯状态，每次触发都直接执行对应动作。

| 外设引脚 | ESP8266 引脚 | 备注 |
|---|---|---|
| 信号线（S） | GPIO14 (D5) | PWM |
| VCC | 5V（VU） | **勿接 3.3V**，否则扭矩不足 |
| GND | GND | — |

### 2.4 ASRPRO 语音模块

离线语音识别模块（唯创知音），固件在**另一个项目**中用天问Block / 天问IDE 开发。本仓库只通过 UART 与之对接：ASRPRO 识别「打开灯光 / 关闭灯光」后发指令，ESP8266 执行并回传结果，ASRPRO 用喇叭播报。

| 外设引脚 | ESP8266 引脚 | 备注 |
|---|---|---|
| ASRPRO TX → ESP RX | GPIO3（D9） | 波特率 9600 |
| ESP TX → ASRPRO RX | GPIO1（D10） | 波特率 9600 |
| VCC / GND | 3.3V / GND | 与 ESP8266 共地 |

## 3. 代码介绍

### 3.1 目录结构

```
src/                       # ESP8266 固件
  main.cpp                 # 初始化 + 主循环
  config.h                 # 引脚 / 阈值 / 协议 / 调试开关（集中配置）
  AppLight.h/.cpp          # 主控制逻辑（自动开灯 + 语音命令，无状态缓存）
  Sensors.h/.cpp           # BH1750 光照（手写 I2C）+ HC-SR04 测距
  ServoSwitch.h/.cpp       # SG90 舵机（开灯 180°/关灯 0°，拨后复位 90°）
  Voice.h/.cpp             # UART 行协议解析 / 响应发送
doc/                       # 需求与设计文档
platformio.ini             # 两个编译环境：esp12f / nodemcuv2
```

### 3.2 依赖库

**无外部依赖库**（`lib_deps` 为空）：

- BH1750：手写软件 I2C 驱动（见 [Sensors.cpp](src/Sensors.cpp)）。
- HC-SR04：手写 `pulseIn` 测距（见 [Sensors.cpp](src/Sensors.cpp)）。
- SG90 舵机：使用 ESP8266 Arduino 内核**自带**的 `Servo` 库。

### 3.3 构建与烧录

PlatformIO 通过 VS Code 的 PlatformIO IDE 扩展安装，`pio` 未必在 `PATH` 上，可用完整路径或扩展 GUI：

```bash
# 构建（默认编译 esp12f 环境）
C:/Users/Admin/.platformio/penv/Scripts/platformio.exe run

# 指定 NodeMCU 开发板环境
C:/Users/Admin/.platformio/penv/Scripts/platformio.exe run -e nodemcuv2

# 构建并烧录
C:/Users/Admin/.platformio/penv/Scripts/platformio.exe run -e nodemcuv2 -t upload

# 打开串口监视器（注意：UART0 被固件设为 9600 接 ASRPRO）
C:/Users/Admin/.platformio/penv/Scripts/platformio.exe device monitor --baud 9600
```

两个环境代码与引脚完全一致，仅 `board` 定义不同（`esp12f` → 裸 ESP-12F 模块，`nodemcuv2` → NodeMCU 开发板）。

## 4. 通信：与 ASRPRO 的 UART 协议

物理层：UART 9600 8N1，文本帧，以换行结尾（兼容 `\n` / `\r\n`）。详细契约见 [doc/软件设计.md](doc/软件设计.md) 第 6 节。

ASRPRO → ESP8266（指令）：

| 帧 | 含义 |
|---|---|
| `ON\n` | 打开灯光 |
| `OFF\n` | 关闭灯光 |

ESP8266 → ASRPRO（响应）：

| 帧 | 含义 | 建议播报语 |
|---|---|---|
| `ON_OK\n` | 已执行开灯 | “已打开灯光” |
| `OFF_OK\n` | 已执行关灯 | “已关闭灯光” |
| `ERROR\n` | 执行出错 | “操作失败” |

> ASRPRO 仅在主动发指令后才等待对应响应；自动感应开灯不回传（此时 ASRPRO 未在等待）。

## 5. 其它

### 串口输出示例

调试日志由 [config.h](src/config.h) 的 `DEBUG_LOG` 控制（默认 `0` 关闭）。置 `1` 后日志输出到 UART0（与 ASRPRO 共用，开发期需先断开 ASRPRO，串口监视器设 **9600**）：

```text
lux=35.2 lx, dist=12.3 cm, cond=0 flag=0
auto on: flag 0->1, lux=35.2 dist=12.3 cm
auto reset: flag 1->0, lux=40.0 dist=80.0 cm
```

### 备注

- 引脚、阈值（`LUX_THRESHOLD=1`、`DIST_THRESHOLD_CM=20`）、舵机角度/延时均以 `#define` 集中在 [config.h](src/config.h)，按实际接线修改。
- 自动开灯的判定为「光照 < 1 lx **且** 距离 < 20 cm」的条件上升沿（触发标记：条件满足且标记无效→置有效并开灯，条件解除→复位）；语音命令无条件执行（每次拨动一次，无状态缓存、不回 `*_SKIP`）。
- 固件不维护灯状态：语音开灯/自动触发拨到 180°、语音关灯拨到 0°，拨后都复位回 90°，每次指令/触发都直接执行。