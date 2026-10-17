#include "ServoSwitch.h"
#include "config.h"

void ServoSwitch::begin() {
  servo_.attach(PIN_SERVO);
  servo_.write(SERVO_ANGLE_RESET);  // 上电停在复位/中位（90°）
  delay(200);
}

void ServoSwitch::on() {
  servo_.write(SERVO_ANGLE_PRESS);  // 复位 90° 顺时针偏转 90° → 180°（开灯）
  delay(SERVO_HOLD_MS);
  servo_.write(SERVO_ANGLE_RESET);  // 复位回中位
  delay(SERVO_SETTLE_MS);
}

void ServoSwitch::off() {
  servo_.write(SERVO_ANGLE_HOME);   // 复位 90° 逆时针偏转 90° → 0°（关灯）
  delay(SERVO_HOLD_MS);
  servo_.write(SERVO_ANGLE_RESET);  // 复位回中位
  delay(SERVO_SETTLE_MS);
}