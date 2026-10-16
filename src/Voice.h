#pragma once
#include <Arduino.h>

enum class VoiceCommand {
  None,
  LightOn,
  LightOff,
};

// UART 行协议：解析 ASRPRO 发来的指令，回传执行结果
class Voice {
public:
  void begin();

  // 非阻塞：从串口缓冲解析出下一条命令；无命令返回 None
  VoiceCommand poll();

  // 回传一条响应帧（自动追加换行）
  void reply(const char *ack);

private:
  VoiceCommand parse(const char *s);

  enum { BufSize = 32 };
  char buf_[BufSize];
  uint8_t len_ = 0;
};