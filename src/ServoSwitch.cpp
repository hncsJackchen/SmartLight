#include "ServoSwitch.h"
#include "config.h"

void ServoSwitch::begin() {
  servo_.attach(PIN_SERVO);
  servo_.write(SERVO_ANGLE_RESET);  // 上电停在回正/中位（90°）
  delay(200);
}

void ServoSwitch::on() {
  servo_.write(SERVO_ANGLE_PRESS);  // 顺时针转 90° 到 180°（开灯）
  delay(SERVO_HOLD_MS);
  servo_.write(SERVO_ANGLE_RESET);  // 回正中位
  delay(SERVO_SETTLE_MS);
}

void ServoSwitch::off() {
  servo_.write(SERVO_ANGLE_HOME);   // 逆时针转 90° 到 0°（关灯）
  delay(SERVO_HOLD_MS);
  servo_.write(SERVO_ANGLE_RESET);  // 回正中位
  delay(SERVO_SETTLE_MS);
}