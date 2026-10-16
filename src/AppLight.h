#pragma once
#include <Arduino.h>
#include "Voice.h"
#include "Sensors.h"
#include "ServoSwitch.h"

// 状态机与主控制逻辑
class AppLight {
public:
  void begin();
  void tick();   // 主循环调用

private:
  enum class LightState : uint8_t { Off, On };

  void handleVoice(VoiceCommand cmd);
  void processAuto();
  void setLightOn(bool on);
  LightState initState();

  Sensors sensors_;
  ServoSwitch switch_;
  Voice voice_;

  LightState state_ = LightState::Off;   // 当前灯的逻辑状态
  bool nearPrev_ = false;                // 进门上升沿检测
  uint32_t lastSample_ = 0;              // 采样节流
  uint32_t lastLog_ = 0;                 // 日志打印节流
};