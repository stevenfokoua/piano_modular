const connectBtn = document.getElementById('connectBtn');
const statusDiv = document.getElementById('status');
const keyButtons = document.querySelectorAll('.key');

let bleDevice = null;
let bleServer = null;
let midiCharacteristic = null;

// MIDI Service and Characteristic UUIDs (BLE MIDI spec)
const MIDI_SERVICE_UUID = '03B80E5A-EDE8-4B33-A751-6CE34EC4C700';
const MIDI_CHARACTERISTIC_UUID = '7772E5DB-3868-4112-A1A9-F2669D106BF3';

connectBtn.addEventListener('click', async () => {
  try {
    statusDiv.textContent = 'Requesting Bluetooth device...';
    bleDevice = await navigator.bluetooth.requestDevice({
      filters: [{ services: [MIDI_SERVICE_UUID] }],
      optionalServices: [MIDI_SERVICE_UUID]
    });
    statusDiv.textContent = 'Connecting...';
    bleServer = await bleDevice.gatt.connect();
    const service = await bleServer.getPrimaryService(MIDI_SERVICE_UUID);
    midiCharacteristic = await service.getCharacteristic(MIDI_CHARACTERISTIC_UUID);
    statusDiv.textContent = 'Connected!';
    connectBtn.disabled = true;
  } catch (err) {
    statusDiv.textContent = 'Connection failed: ' + err;
  }
});

function sendMidiNote(note, on) {
  if (!midiCharacteristic) return;
  // BLE MIDI packet: [header, timestamp, status, note, velocity]
  // 0x90 = Note On, 0x80 = Note Off, channel 0
  const status = on ? 0x90 : 0x80;
  const velocity = on ? 127 : 0;
  const packet = new Uint8Array([0x80, 0x80, status, note, velocity]);
  midiCharacteristic.writeValue(packet);
}

keyButtons.forEach(btn => {
  btn.addEventListener('mousedown', () => {
    const note = parseInt(btn.dataset.note, 10);
    sendMidiNote(note, true);
  });
  btn.addEventListener('mouseup', () => {
    const note = parseInt(btn.dataset.note, 10);
    sendMidiNote(note, false);
  });
  btn.addEventListener('mouseleave', () => {
    const note = parseInt(btn.dataset.note, 10);
    sendMidiNote(note, false);
  });
  // Touch support for mobile
  btn.addEventListener('touchstart', e => {
    e.preventDefault();
    const note = parseInt(btn.dataset.note, 10);
    sendMidiNote(note, true);
  });
  btn.addEventListener('touchend', e => {
    e.preventDefault();
    const note = parseInt(btn.dataset.note, 10);
    sendMidiNote(note, false);
  });
});
