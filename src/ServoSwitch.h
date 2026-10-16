#pragma once
#include <Arduino.h>
#include <Servo.h>

// SG90 舵机：通过「转 90° → 回正」拨动实体灯光开关（toggle）
class ServoSwitch {
public:
  void begin();
  void press();   // 拨动一次开关
private:
  Servo servo_;
};