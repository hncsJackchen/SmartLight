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
      switch_.press();          // 无条件拨动一次，不判断当前状态
      voice_.reply(ACK_ON_OK);
      break;
    case VoiceCommand::LightOff:
      switch_.press();
      voice_.reply(ACK_OFF_OK);
      break;
    default:
      break;
  }
}

void AppLight::processAuto() {
  float lux = sensors_.readLightLux();
  float dist = sensors_.readDistanceCm();

  // 定期打印采集数据（受 DEBUG_LOG 控制）
  uint32_t now = millis();
  if (now - lastLog_ >= LOG_INTERVAL_MS) {
    lastLog_ = now;
    LOG("lux=%.1f lx, dist=%.1f cm", lux, dist);
  }

  bool near = (dist >= 0 && dist < DIST_THRESHOLD_CM);      // 有人贴近
  bool dark = (lux >= 0 && lux < LUX_THRESHOLD);            // 环境暗

  // 进门上升沿 且 环境暗 -> 拨动开灯（无状态，每次进门都拨）
  if (near && !nearPrev_ && dark) {
    LOG("auto on: lux=%.1f dist=%.1f cm", lux, dist);
    switch_.press();
  }
  nearPrev_ = near;
}