#include "ServoSwitch.h"
#include "config.h"

void ServoSwitch::begin() {
  servo_.attach(PIN_SERVO);
  servo_.write(SERVO_ANGLE_HOME);
  delay(200);
}

void ServoSwitch::press() {
  servo_.write(SERVO_ANGLE_PRESS);
  delay(SERVO_HOLD_MS);
  servo_.write(SERVO_ANGLE_HOME);
  delay(SERVO_SETTLE_MS);
}