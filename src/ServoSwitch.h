#pragma once
#include <Arduino.h>
#include <Servo.h>

// SG90 舵机：开灯顺时针转到 SERVO_ANGLE_PRESS(180°)，关灯逆时针转到 SERVO_ANGLE_HOME(0°)
class ServoSwitch {
public:
  void begin();
  void on();    // 开灯：转到 180°
  void off();   // 关灯：转到 0°
private:
  Servo servo_;
};