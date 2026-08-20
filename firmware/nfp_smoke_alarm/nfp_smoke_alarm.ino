#include <ADC121C021.h>
#include <Arduino.h>
#include <Wire.h>

namespace {

constexpr std::uint8_t kMq2Address = 0x51;
constexpr float kCleanAirRatio = 1.0F;
constexpr float kLoadResistanceKohm = 10.0F;
constexpr float kAlarmThresholdPpm = 3000.0F;

ADC121C021 smokeSensor;

bool initializeSensor() {
  pinMode(WB_IO6, OUTPUT);
  digitalWrite(WB_IO6, HIGH);
  delay(500);
  if (!smokeSensor.begin(kMq2Address, Wire)) return false;
  smokeSensor.setRL(kLoadResistanceKohm);
  smokeSensor.setA(-0.98F);
  smokeSensor.setB(1.154F);
  smokeSensor.setRegressionMethod(0);

  float calibration = 0;
  constexpr int kSamples = 50;
  for (int sample = 0; sample < kSamples; ++sample) {
    calibration += smokeSensor.calibrateR0(kCleanAirRatio);
    delay(20);
  }
  smokeSensor.setR0(calibration / kSamples);
  return isfinite(smokeSensor.getR0()) && smokeSensor.getR0() > 0;
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(WB_IO1, OUTPUT);
  if (!initializeSensor()) {
    Serial.println("MQ-2 initialization failed. Check power, I2C, and clean-air calibration.");
  }
}

void loop() {
  const float smokePpm = smokeSensor.readSensor();
  const bool alarm = isfinite(smokePpm) && smokePpm >= kAlarmThresholdPpm;
  digitalWrite(WB_IO1, alarm ? HIGH : LOW);
  Serial.printf("smoke_ppm=%.2f alarm=%s\n", smokePpm, alarm ? "true" : "false");
  delay(1000);
}
