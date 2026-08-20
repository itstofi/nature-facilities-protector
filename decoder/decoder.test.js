const assert = require('node:assert/strict');
const { decodeEnvironment } = require('./decoder');

const expected = {
  messageType: 1,
  temperatureC: 28.25,
  humidityPercent: 50,
  pressureHpa: 958.57,
  gasResistanceOhm: 100406,
};

assert.deepEqual(decodeEnvironment('010b0913880001767100018836'), expected);
assert.deepEqual(decodeEnvironment('010b09138800017671000188360f0a'), {
  ...expected,
  batteryMv: 3850,
});
const validBytes = [...Buffer.from('010b0913880001767100018836', 'hex')];
assert.deepEqual(decodeEnvironment(validBytes), expected);
for (const malformed of [
  Object.assign([...validBytes], { 2: 256 }),
  Object.assign([...validBytes], { 2: -1 }),
  Object.assign([...validBytes], { 2: 1.5 }),
  Object.assign([...validBytes], { 2: undefined }),
  new Uint16Array(validBytes),
]) {
  assert.throws(() => decodeEnvironment(malformed), /byte|Uint8Array/i);
}
const sparse = [...validBytes];
delete sparse[2];
assert.throws(() => decodeEnvironment(sparse), /byte/i);

assert.throws(() => decodeEnvironment('01' + '00'.repeat(12)), /sensor bounds/i);
assert.throws(() => decodeEnvironment('01' + 'ff'.repeat(12)), /sensor bounds/i);
assert.throws(() => decodeEnvironment('02' + '00'.repeat(12)), /message type/i);
assert.throws(() => decodeEnvironment('0100'), /13 or 15 bytes/i);
assert.throws(() => decodeEnvironment('01' + '00'.repeat(13)), /13 or 15 bytes/i);
console.log('JavaScript decoder tests passed.');
