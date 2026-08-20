#include <cassert>
#include <cmath>
#include <iostream>

#include "smoke_policy.h"

int main() {
  assert(nfp::classifySmoke(false, 10.0F, 3000.0F) == nfp::SmokeState::SensorFault);
  assert(nfp::classifySmoke(true, NAN, 3000.0F) == nfp::SmokeState::SensorFault);
  assert(nfp::classifySmoke(true, 2999.9F, 3000.0F) == nfp::SmokeState::Normal);
  assert(nfp::classifySmoke(true, 3000.0F, 3000.0F) == nfp::SmokeState::Alarm);
  assert(nfp::shouldAssertAlarm(nfp::SmokeState::SensorFault));
  assert(nfp::shouldAssertAlarm(nfp::SmokeState::Alarm));
  assert(!nfp::shouldAssertAlarm(nfp::SmokeState::Normal));
  std::cout << "C++ smoke policy tests passed.\n";
  return 0;
}
