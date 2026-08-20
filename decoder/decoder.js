'use strict';

const ENVIRONMENT_MESSAGE_TYPE = 0x01;
const LEGACY_PAYLOAD_SIZE = 13;
const EXTENDED_PAYLOAD_SIZE = 15;

function toBuffer(payload) {
  if (Buffer.isBuffer(payload)) return payload;
  if (Array.isArray(payload) || ArrayBuffer.isView(payload)) return Buffer.from(payload);
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
  return reading;
}

module.exports = {
  decodeEnvironment,
  ENVIRONMENT_MESSAGE_TYPE,
  LEGACY_PAYLOAD_SIZE,
  EXTENDED_PAYLOAD_SIZE,
};
