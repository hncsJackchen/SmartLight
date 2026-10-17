#pragma once
#include <Arduino.h>

// ===== 引脚分配（ESP-12F / NodeMCU，GPIO 编号）=====
#define PIN_SERVO           14   // D5 — SG90 舵机信号
#define PIN_LIGHT_SDA       4    // D2 — BH1750 数据
#define PIN_LIGHT_SCL       5    // D1 — BH1750 时钟
#define PIN_SR04_TRIG       13   // D7 — HC-SR04 触发
#define PIN_SR04_ECHO       12   // D6 — HC-SR04 回波（3.3V 供电直连）
#define PIN_LED             2    // D4 — 板载 LED（低电平点亮，运行时常亮）

// ===== 阈值 =====
#define LUX_THRESHOLD       1.0f     // 光照阈值 (lx)，< 此值视为「暗」
#define DIST_THRESHOLD_CM   20.0f    // 进门距离阈值 (cm)
#define DIST_MIN_VALID_CM   2.0f     // HC-SR04 盲区，小于此值判无效

// ===== 舵机动作 =====
#define SERVO_ANGLE_PRESS   180    // 开灯角度 = 复位 90° 顺时针偏转 90°（绝对 180°）
#define SERVO_ANGLE_HOME    0      // 关灯角度 = 复位 90° 逆时针偏转 90°（绝对 0°）
#define SERVO_ANGLE_RESET   90     // 复位/中位角度（拨动后回到此位置）
#define SERVO_HOLD_MS       450    // 拨到位后停留时间
#define SERVO_SETTLE_MS     300    // 复位后稳定时间

// ===== 语音串口 =====
// UART0：TX = GPIO1（D10）、RX = GPIO3（D9），接 ASRPRO
#define VOICE_BAUDRATE      9600   // 天问默认 9600

// 协议帧（见 doc/软件设计.md 第 6 节）
#define CMD_LIGHT_ON        "ON"
#define CMD_LIGHT_OFF       "OFF"
#define ACK_ON_OK           "ON_OK"
#define ACK_OFF_OK          "OFF_OK"
#define ACK_ERROR           "ERROR"

// ===== 采样 =====
#define SENSOR_INTERVAL_MS  100
#define LOG_INTERVAL_MS     1000  // 传感器数据日志打印间隔

// ===== 调试日志（UART0 与 ASRPRO 共用，开发期可开，量产保持 0）=====
#define DEBUG_LOG           1

#if DEBUG_LOG
#define LOG(fmt, ...) do { Serial.printf_P(PSTR(fmt "\r\n"), ##__VA_ARGS__); } while (0)
#else
#define LOG(fmt, ...) do {} while (0)
#endif