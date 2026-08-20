#pragma once

#include <cmath>

namespace nfp {

enum class SmokeState { Normal, Alarm, SensorFault };

inline SmokeState classifySmoke(bool sensorReady, float smokePpm, float alarmThresholdPpm) {
  if (!sensorReady || !std::isfinite(smokePpm) || !std::isfinite(alarmThresholdPpm) ||
      alarmThresholdPpm <= 0.0F) {
    return SmokeState::SensorFault;
  }
  return smokePpm >= alarmThresholdPpm ? SmokeState::Alarm : SmokeState::Normal;
}

inline bool shouldAssertAlarm(SmokeState state) { return state != SmokeState::Normal; }

}  // namespace nfp
