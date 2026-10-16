#pragma once
#include <Arduino.h>
#include "Voice.h"
#include "Sensors.h"
#include "ServoSwitch.h"

// 主控制逻辑：无状态缓存，每次语音指令 / 传感器触发都直接拨动，不判断灯当前状态
class AppLight {
public:
  void begin();
  void tick();   // 主循环调用

private:
  void handleVoice(VoiceCommand cmd);
  void processAuto();

  Sensors sensors_;
  ServoSwitch switch_;
  Voice voice_;

  bool nearPrev_ = false;                // 进门上升沿检测
  uint32_t lastSample_ = 0;              // 采样节流
  uint32_t lastLog_ = 0;                 // 日志打印节流
};