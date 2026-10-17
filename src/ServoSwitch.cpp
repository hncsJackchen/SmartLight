#include "ServoSwitch.h"
#include "config.h"

void ServoSwitch::begin() {
  servo_.attach(PIN_SERVO);
  servo_.write(SERVO_ANGLE_HOME);   // 上电停在关灯位置（0°）
  delay(200);
}

void ServoSwitch::on() {
  servo_.write(SERVO_ANGLE_PRESS);  // 顺时针转到 180°（开灯）
  delay(SERVO_HOLD_MS);
}

void ServoSwitch::off() {
  servo_.write(SERVO_ANGLE_HOME);   // 逆时针转到 0°（关灯）
  delay(SERVO_HOLD_MS);
}