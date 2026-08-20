'use strict';

const ENVIRONMENT_MESSAGE_TYPE = 0x01;
const LEGACY_PAYLOAD_SIZE = 13;
const EXTENDED_PAYLOAD_SIZE = 15;

function toBuffer(payload) {
  if (Buffer.isBuffer(payload)) return payload;
  if (Array.isArray(payload)) {
    const length = payload.length;
    const bytes = Buffer.alloc(length);
    for (let index = 0; index < length; index += 1) {
      if (!Object.hasOwn(payload, index)) {
        throw new TypeError('payload arrays must contain explicit byte integers from 0 to 255');
      }
      const value = payload[index];
      if (!Number.isInteger(value) || value < 0 || value > 255) {
        throw new TypeError('payload arrays must contain explicit byte integers from 0 to 255');
      }
      bytes[index] = value;
    }
    return bytes;
  }
  if (payload instanceof Uint8Array) return Buffer.from(payload);
  if (ArrayBuffer.isView(payload)) {
    throw new TypeError('typed payload views must be Uint8Array or Buffer');
  }
  if (typeof payload === 'string') {
    if (!/^[0-9a-fA-F]+$/.test(payload) || payload.length % 2 !== 0) {
      throw new TypeError('payload hex must contain complete hexadecimal bytes');
    }
    return Buffer.from(payload, 'hex');
  }
  throw new TypeError('payload must be a hex string, byte array, or Buffer');
}

function decodeEnvironment(payload) {
  const bytes = toBuffer(payload);
  if (bytes.length !== LEGACY_PAYLOAD_SIZE && bytes.length !== EXTENDED_PAYLOAD_SIZE) {
    throw new RangeError('environment payload must be exactly 13 or 15 bytes');
  }
  if (bytes.readUInt8(0) !== ENVIRONMENT_MESSAGE_TYPE) {
    throw new RangeError(`unsupported message type: ${bytes.readUInt8(0)}`);
  }
  const reading = {
    messageType: ENVIRONMENT_MESSAGE_TYPE,
    temperatureC: bytes.readUInt16BE(1) / 100,
    humidityPercent: bytes.readUInt16BE(3) / 100,
    pressureHpa: bytes.readUInt32BE(5) / 100,
    gasResistanceOhm: bytes.readUInt32BE(9),
  };
  if (bytes.length === EXTENDED_PAYLOAD_SIZE) reading.batteryMv = bytes.readUInt16BE(13);
  if (
    reading.temperatureC < 0 || reading.temperatureC > 85 ||
    reading.humidityPercent < 0 || reading.humidityPercent > 100 ||
    reading.pressureHpa < 300 || reading.pressureHpa > 1100 ||
    reading.gasResistanceOhm < 1 || reading.gasResistanceOhm > 100000000 ||
    ('batteryMv' in reading && (reading.batteryMv < 0 || reading.batteryMv > 6000))
  ) {
    throw new RangeError('environment reading is outside supported sensor bounds');
  }
  return reading;
}

module.exports = {
  decodeEnvironment,
  ENVIRONMENT_MESSAGE_TYPE,
  LEGACY_PAYLOAD_SIZE,
  EXTENDED_PAYLOAD_SIZE,
};
