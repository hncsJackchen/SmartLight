#include "Sensors.h"
#include "config.h"
#include <Wire.h>

// BH1750 寄存器命令
static constexpr uint8_t BH_POWER_ON   = 0x01; // 上电
static constexpr uint8_t BH_CONT_H_RES = 0x10; // 连续高分辨率模式 (1 lx, 120ms)
static constexpr float   BH_DIV        = 1.2f; // 分辨率换算系数

void Sensors::begin() {
  // ESP8266 的 Wire 为软件 I2C，可指定任意两只引脚
  Wire.begin(PIN_LIGHT_SDA, PIN_LIGHT_SCL);

  lightAddr_ = probeLightAddr();
  if (lightAddr_ != 0) {
    writeBh1750(BH_POWER_ON);
    delay(10);
    writeBh1750(BH_CONT_H_RES);
    delay(120);
  }

  pinMode(PIN_SR04_TRIG, OUTPUT);
  pinMode(PIN_SR04_ECHO, INPUT);
  digitalWrite(PIN_SR04_TRIG, LOW);
}

uint8_t Sensors::probeLightAddr() {
  for (uint8_t addr : {0x23, 0x5C}) {   // ADDR=低 0x23 / ADDR=高 0x5C
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) return addr;
  }
  return 0;
}

bool Sensors::writeBh1750(uint8_t cmd) {
  Wire.beginTransmission(lightAddr_);
  Wire.write(cmd);
  return Wire.endTransmission() == 0;
}

bool Sensors::readBh1750(uint16_t &raw) {
  Wire.requestFrom(static_cast<int>(lightAddr_), static_cast<uint8_t>(2));
  uint32_t t = millis();
  while (Wire.available() < 2) {
    if (millis() - t > 50) return false;
  }
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  raw = (static_cast<uint16_t>(hi) << 8) | lo;
  return true;
}

float Sensors::readLightLux() {
  if (lightAddr_ == 0) return -1;
  uint16_t raw = 0;
  if (!readBh1750(raw)) return -1;
  return raw / BH_DIV;
}

float Sensors::readDistanceCm() {
  // 触发 10us 高电平脉冲
  digitalWrite(PIN_SR04_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_SR04_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SR04_TRIG, LOW);

  // 测量回波脉宽（约 30ms 超时 ≈ 5m 量程）
  long us = pulseIn(PIN_SR04_ECHO, HIGH, 30000UL);
  if (us == 0) return -1;                        // 超时（前方无反射物）
  float cm = us * 0.0343f / 2.0f;                // 声速 343 m/s
  if (cm < DIST_MIN_VALID_CM) return -1;         // 丢弃盲区/串扰的假回波
  return cm;
}