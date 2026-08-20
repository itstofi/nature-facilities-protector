#include <Adafruit_BME680.h>
#include <Arduino.h>
#include <LoRaWan-RAK4630.h>
#include <Wire.h>

#include <algorithm>

#include "nfp_payload.h"
#include "credentials.h"

namespace {

constexpr LoRaMacRegion_t kRegion = LORAMAC_REGION_EU868;
constexpr DeviceClass_t kDeviceClass = CLASS_A;
constexpr std::uint8_t kAppPort = LORAWAN_APP_PORT;
constexpr std::uint32_t kTransmitIntervalMs = 5UL * 60UL * 1000UL;
constexpr std::size_t kPayloadCapacity = 64;

Adafruit_BME680 sensor;
TimerEvent_t transmitTimer;
std::array<std::uint8_t, kPayloadCapacity> packetBuffer{};
lmh_app_data_t packet{packetBuffer.data(), 0, kAppPort, 0, 0};

void onJoined();
void onJoinFailed();
void onReceive(lmh_app_data_t* data);
void onClassConfirmed(DeviceClass_t deviceClass);
void onTransmitTimer();
void sendEnvironmentReading();

lmh_callback_t callbacks = {
    BoardGetBatteryLevel,
    BoardGetUniqueId,
    BoardGetRandomSeed,
    onReceive,
    onJoined,
    onClassConfirmed,
    onJoinFailed,
};

lmh_param_t parameters = {
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    3,
    TX_POWER_5,
    LORAWAN_DUTYCYCLE_ON,
};

template <std::size_t N>
bool hasNonZero(const std::uint8_t (&value)[N]) {
  for (std::uint8_t byte : value) {
    if (byte != 0) return true;
  }
  return false;
}

bool credentialsAreConfigured() {
  return hasNonZero(NFP_DEVICE_EUI) && hasNonZero(NFP_APP_EUI) && hasNonZero(NFP_APP_KEY);
}

std::uint16_t readBatteryMv() {
#ifdef WB_A0
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);
  constexpr float kMvPerLsb = 3000.0F / 4096.0F;
  constexpr float kDivider = 1.73F;
  return static_cast<std::uint16_t>(analogRead(WB_A0) * kMvPerLsb * kDivider);
#else
  return 0;
#endif
}

void scheduleNextTransmission() {
  TimerSetValue(&transmitTimer, kTransmitIntervalMs);
  TimerStart(&transmitTimer);
}

void onJoined() {
  Serial.println("LoRaWAN joined; scheduling environmental uplinks.");
  if (lmh_class_request(kDeviceClass) == LMH_SUCCESS) scheduleNextTransmission();
}

void onJoinFailed() {
  Serial.println("LoRaWAN join failed. Check local credentials, region, and gateway coverage.");
}

void onReceive(lmh_app_data_t* data) {
  Serial.printf("Downlink port=%u bytes=%u rssi=%d snr=%d\n", data->port, data->buffsize,
                data->rssi, data->snr);
}

void onClassConfirmed(DeviceClass_t deviceClass) {
  Serial.printf("LoRaWAN class %c confirmed.\n", "ABC"[deviceClass]);
}

void onTransmitTimer() {
  sendEnvironmentReading();
  scheduleNextTransmission();
}

void sendEnvironmentReading() {
  if (lmh_join_status_get() != LMH_SET) {
    Serial.println("Skipping uplink because the node is not joined.");
    return;
  }
  if (!sensor.performReading()) {
    Serial.println("BME680 reading failed.");
    return;
  }

  if (!isfinite(sensor.temperature) || !isfinite(sensor.humidity) ||
      !isfinite(sensor.pressure) || !isfinite(sensor.gas_resistance) ||
      sensor.temperature < 0.0F || sensor.temperature > 655.35F ||
      sensor.humidity < 0.0F || sensor.humidity > 100.0F || sensor.pressure < 0.0F ||
      sensor.gas_resistance < 0.0F) {
    Serial.println("Skipping invalid or historically unencodable sensor reading.");
    return;
  }
  const nfp::EnvironmentReading reading{
      static_cast<std::uint16_t>(sensor.temperature * 100.0F),
      static_cast<std::uint16_t>(sensor.humidity * 100.0F),
      static_cast<std::uint32_t>((sensor.pressure / 100.0F) * 100.0F),
      static_cast<std::uint32_t>(sensor.gas_resistance),
      readBatteryMv(),
  };
  std::array<std::uint8_t, nfp::kExtendedPayloadSize> encoded{};
  if (!nfp::encodeExtendedEnvironment(reading, encoded)) {
    Serial.println("Skipping reading rejected by payload validation.");
    return;
  }
  std::copy(encoded.begin(), encoded.end(), packetBuffer.begin());
  packet.buffsize = encoded.size();
  packet.port = kAppPort;

  const lmh_error_status result = lmh_send(&packet, LMH_UNCONFIRMED_MSG);
  Serial.printf("Environmental uplink %s (%u bytes).\n",
                result == LMH_SUCCESS ? "queued" : "failed", packet.buffsize);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  const unsigned long started = millis();
  while (!Serial && millis() - started < 5000) delay(50);

  if (!credentialsAreConfigured()) {
    Serial.println("Credentials are placeholders. Copy credentials.example.h to credentials.h.");
    return;
  }

  Wire.begin();
  if (!sensor.begin(0x76)) {
    Serial.println("BME680 not detected at I2C address 0x76.");
    return;
  }
  sensor.setTemperatureOversampling(BME680_OS_8X);
  sensor.setHumidityOversampling(BME680_OS_2X);
  sensor.setPressureOversampling(BME680_OS_4X);
  sensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
  sensor.setGasHeater(320, 150);

  lora_rak4630_init();
  lmh_setDevEui(const_cast<std::uint8_t*>(NFP_DEVICE_EUI));
  lmh_setAppEui(const_cast<std::uint8_t*>(NFP_APP_EUI));
  lmh_setAppKey(const_cast<std::uint8_t*>(NFP_APP_KEY));
  if (lmh_init(&callbacks, parameters, true, kDeviceClass, kRegion) != 0) {
    Serial.println("LoRaWAN initialization failed.");
    return;
  }

  TimerInit(&transmitTimer, onTransmitTimer);
  lmh_join();
}

void loop() { Radio.IrqProcess(); }
