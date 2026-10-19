#pragma once
#include <Arduino.h>
#include <Servo.h>

// SG90 舵机：开灯从中位 90° 顺时针转 90°（到 180°）、关灯逆时针转 90°（到 0°），拨后回中位 90°
class ServoSwitch {
public:
  void begin();
  void on();    // 开灯：90° 顺转 90°（到 180°）→ 回 90°
  void off();   // 关灯：90° 逆转 90°（到 0°）→ 回 90°
private:
  Servo servo_;
};