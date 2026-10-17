#pragma once
#include <Arduino.h>
#include <Servo.h>

// SG90 舵机：开灯拨到 180°、关灯拨到 0°，拨完都复位回中位 90°
class ServoSwitch {
public:
  void begin();
  void on();    // 开灯：180° → 复位 90°
  void off();   // 关灯：0° → 复位 90°
private:
  Servo servo_;
};