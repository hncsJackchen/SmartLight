#include "AppLight.h"
#include "config.h"

void AppLight::begin() {
  // 运行指示灯：板载 LED 低电平点亮，此处拉低=常亮（boot 已完成，不影响 GPIO2 启动）
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  sensors_.begin();
  switch_.begin();
  voice_.begin();
  state_ = initState();
  LOG("boot, initial light=%d", static_cast<int>(state_));
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

AppLight::LightState AppLight::initState() {
  // 采样数次取平均，减少首采抖动
  float sum = 0;
  int n = 0;
  for (int i = 0; i < 3; i++) {
    float lx = sensors_.readLightLux();
    if (lx >= 0) { sum += lx; n++; }
    delay(50);
  }
  if (n == 0) return LightState::Off;   // 传感器失败，按“关”处理
  return (sum / n) >= LUX_THRESHOLD ? LightState::On : LightState::Off;
}

void AppLight::handleVoice(VoiceCommand cmd) {
  switch (cmd) {
    case VoiceCommand::LightOn:
      if (state_ == LightState::On) {
        voice_.reply(ACK_ON_SKIP);
      } else {
        setLightOn(true);
        voice_.reply(ACK_ON_OK);
      }
      break;
    case VoiceCommand::LightOff:
      if (state_ == LightState::Off) {
        voice_.reply(ACK_OFF_SKIP);
      } else {
        setLightOn(false);
        voice_.reply(ACK_OFF_OK);
      }
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

  // 进门上升沿 且 环境暗 且 灯当前为关 -> 自动开灯
  if (near && !nearPrev_ && dark && state_ == LightState::Off) {
    LOG("auto on: lux=%.1f dist=%.1f cm", lux, dist);
    setLightOn(true);
  }
  nearPrev_ = near;
}

void AppLight::setLightOn(bool on) {
  LightState target = on ? LightState::On : LightState::Off;
  if (state_ == target) return;   // 幂等：状态未变则不拨动

  switch_.press();                // toggle 实体开关
  state_ = target;
  LOG("light -> %d", static_cast<int>(state_));
}