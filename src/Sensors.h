#pragma once
#include <Arduino.h>

// 传感器封装：BH1750 光照（软件 I2C）+ HC-SR04 超声波测距
class Sensors {
public:
  void begin();

  // 环境照度 (lx)；失败返回 -1
  float readLightLux();

  // 距离 (cm)；超时/无回波返回 -1
  float readDistanceCm();

  // 是否探测到光传感器
  bool hasLight() const { return lightAddr_ != 0; }

private:
  uint8_t probeLightAddr();
  bool writeBh1750(uint8_t cmd);
  bool readBh1750(uint16_t &raw);

  uint8_t lightAddr_ = 0;   // 0 表示未探测到 I2C 设备
};