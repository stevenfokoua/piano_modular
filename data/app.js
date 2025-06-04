const connectBtn = document.getElementById('connectBtn');
const statusDiv = document.getElementById('status');
const keyButtons = document.querySelectorAll('.key');
const colorPickers = document.querySelectorAll('.color-picker');
const presetButtons = document.querySelectorAll('.preset');
const presetSelect = document.getElementById('presetSelect');
const customPattern = document.getElementById('customPattern');
const noteInputs = document.querySelectorAll('.note-input');
const applyPatternBtn = document.getElementById('applyPattern');

let ws = null;
let wsConnected = false;
let reconnectInterval = null;
let midiNotes = [60, 62, 64]; // Default: C D E

function updateStatus(text, color = "#555") {
  statusDiv.textContent = text;
  statusDiv.style.color = color;
}

// Robust WebSocket connection with auto-reconnect
function connectWebSocket() {
  if (ws && wsConnected) return;
  updateStatus('Connecting...', "#fb8c00");
  ws = new WebSocket('ws://' + location.hostname + '/ws');

  ws.onopen = () => {
    wsConnected = true;
    updateStatus('WebSocket connected!', "#388e3c");
    connectBtn.disabled = true;
    if (reconnectInterval) {
      clearInterval(reconnectInterval);
      reconnectInterval = null;
    }
  };

  ws.onclose = () => {
    wsConnected = false;
    updateStatus('WebSocket disconnected. Reconnecting...', "#e53935");
    connectBtn.disabled = false;
    if (!reconnectInterval) {
      reconnectInterval = setInterval(connectWebSocket, 2000);
    }
  };

  ws.onerror = (err) => {
    wsConnected = false;
    updateStatus('WebSocket error. Reconnecting...', "#e53935");
    connectBtn.disabled = false;
    if (!reconnectInterval) {
      reconnectInterval = setInterval(connectWebSocket, 2000);
    }
  };
}

connectBtn.addEventListener('click', connectWebSocket);

// Send color command via WebSocket
function sendColor(button, color) {
  if (!wsConnected) return;
  const msg = {
    type: "setColor",
    button: button,
    color: color
  };
  ws.send(JSON.stringify(msg));
}

// Color picker events
colorPickers.forEach(picker => {
  picker.addEventListener('input', (e) => {
    const button = parseInt(picker.dataset.button, 10);
    sendColor(button, picker.value);
  });
});

// Preset color buttons
presetButtons.forEach(btn => {
  btn.addEventListener('click', () => {
    const button = parseInt(btn.dataset.button, 10);
    const color = btn.dataset.color;
    sendColor(button, color);
    // Also update the color picker to match
    document.querySelector(`.color-picker[data-button="${button}"]`).value = color;
  });
});

// Pattern/preset logic
presetSelect.addEventListener('change', () => {
  if (presetSelect.value === "custom") {
    customPattern.style.display = "";
  } else {
    customPattern.style.display = "none";
    if (presetSelect.value === "c_major") {
      midiNotes = [60, 62, 64];
    } else if (presetSelect.value === "pentatonic") {
      midiNotes = [60, 62, 67];
    }
    updateKeyLabels();
  }
});

noteInputs.forEach(input => {
  input.addEventListener('input', () => {
    const button = parseInt(input.dataset.button, 10);
    midiNotes[button] = parseInt(input.value, 10);
    updateKeyLabels();
  });
});

applyPatternBtn.addEventListener('click', () => {
  // For demo: just update the key labels and internal mapping
  updateKeyLabels();
  updateStatus('Pattern applied!', "#388e3c");
});

function updateKeyLabels() {
  keyButtons.forEach((btn, i) => {
    btn.textContent = midiNoteToName(midiNotes[i]);
    btn.dataset.note = midiNotes[i];
  });
}

function midiNoteToName(note) {
  const names = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
  const n = parseInt(note, 10);
  if (isNaN(n) || n < 21 || n > 108) return n;
  const octave = Math.floor((n / 12)) - 1;
  return names[n % 12] + octave;
}

// (Optional) Key press events for MIDI (if you want to keep this feature)
keyButtons.forEach((btn, i) => {
  btn.addEventListener('mousedown', () => {
    // MIDI note logic could go here if needed
  });
  btn.addEventListener('mouseup', () => {
    // MIDI note logic could go here if needed
  });
  btn.addEventListener('mouseleave', () => {
    // MIDI note logic could go here if needed
  });
  // Touch support for mobile
  btn.addEventListener('touchstart', e => {
    e.preventDefault();
    // MIDI note logic could go here if needed
  });
  btn.addEventListener('touchend', e => {
    e.preventDefault();
    // MIDI note logic could go here if needed
  });
});

// Responsive: update on load
updateKeyLabels();
