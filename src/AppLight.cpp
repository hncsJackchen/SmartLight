#include "AppLight.h"
#include "config.h"

void AppLight::begin() {
  // 运行指示灯：板载 LED 低电平点亮，此处拉低=常亮（boot 已完成，不影响 GPIO2 启动）
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  sensors_.begin();
  switch_.begin();
  voice_.begin();
}

void AppLight::tick() {
  VoiceCommand cmd = voice_.poll();
  if (cmd != VoiceCommand::None) handleVoice(cmd);

  uint32_t now = millis();
  if (now - lastSample_ >= SENSOR_INTERVAL_MS) {
    lastSample_ = now;
    processAuto();
  }
}

void AppLight::handleVoice(VoiceCommand cmd) {
  switch (cmd) {
    case VoiceCommand::LightOn:
      switch_.on();          // 开灯：中位 90° 顺转 90°（到 180°）再回正
      voice_.reply(ACK_ON_OK);
      break;
    case VoiceCommand::LightOff:
      switch_.off();         // 关灯：中位 90° 逆转 90°（到 0°）再回正
      voice_.reply(ACK_OFF_OK);
      break;
    default:
      break;
  }
}

void AppLight::processAuto() {
  float lux = sensors_.readLightLux();
  float dist = sensors_.readDistanceCm();

  bool distValid = (dist >= 0);                             // 测距有效（-1 表示无效/超时）
  bool near = (distValid && dist < DIST_THRESHOLD_CM);      // 有人贴近
  bool dark = (lux >= 0 && lux < LUX_THRESHOLD);            // 环境暗
  bool cond = near && dark;                                 // 触发条件

  // 定期打印采集与触发标记状态（受 DEBUG_LOG 控制）
  uint32_t now = millis();
  if (now - lastLog_ >= LOG_INTERVAL_MS) {
    lastLog_ = now;
    LOG("lux=%.1f lx, dist=%.1f cm, cond=%d flag=%d", lux, dist, (int)cond, (int)trigFlag_);
  }

  // 触发标记：条件满足且仍无效 -> 置有效并开灯；测距有效且条件解除才复位（测距无效不重置）
  if (cond && !trigFlag_) {
    trigFlag_ = true;
    LOG("auto on: flag 0->1, lux=%.1f dist=%.1f cm", lux, dist);
    switch_.on();
  } else if (distValid && !cond && trigFlag_) {
    trigFlag_ = false;
    LOG("auto reset: flag 1->0, lux=%.1f dist=%.1f cm", lux, dist);
  }
}