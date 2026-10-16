#include "Voice.h"
#include "config.h"

void Voice::begin() {
  Serial.begin(VOICE_BAUDRATE);
  len_ = 0;
}

VoiceCommand Voice::poll() {
  while (Serial.available()) {
    char c = static_cast<char>(Serial.read());
    if (c == '\n' || c == '\r') {
      if (len_ > 0) {
        buf_[len_] = '\0';
        len_ = 0;
        VoiceCommand cmd = parse(buf_);
        if (cmd != VoiceCommand::None) return cmd;
      }
      continue;
    }
    if (len_ < BufSize - 1) buf_[len_++] = c;
  }
  return VoiceCommand::None;
}

void Voice::reply(const char *ack) {
  Serial.print(ack);
  Serial.print('\n');
}

VoiceCommand Voice::parse(const char *s) {
  if (strcmp(s, CMD_LIGHT_ON) == 0)  return VoiceCommand::LightOn;
  if (strcmp(s, CMD_LIGHT_OFF) == 0) return VoiceCommand::LightOff;
  return VoiceCommand::None;
}