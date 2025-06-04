# INT7 Modular Floor Piano

**Educational & Therapeutic Floor Piano Kit**  
A robust, modular, and programmable floor piano for children, designed for education, therapy, and creative play.

---

## Overview

The INT7 Modular Floor Piano is an open-source hardware/software project that empowers children to build, program, and play a large-scale, interactive piano. It is designed for classrooms, therapy, and STEAM education, with a focus on durability, safety, and extensibility.

- **Hardware:** ESP32-S3 based, modular key system, WS2812B LED feedback, robust pushbuttons.
- **Software:** Arduino (PlatformIO), BLE MIDI, real-time LED control, web-based programming interface (mBlock compatible).
- **Connectivity:** BLE MIDI, WiFi AP mode, WebSocket server, USB programming.

---

## Features

- **Durable, Modular Hardware:** Designed for repeated assembly/disassembly and child-safe operation.
- **Programmable:** Open system, programmable via mBlock or Arduino IDE.
- **MIDI Output:** BLE MIDI for use with GarageBand and other DAWs.
- **Visual Feedback:** Per-key RGB LED feedback and programmable light shows.
- **Web App:** Responsive, mobile-friendly interface for real-time control and pattern programming.
- **Low Latency:** Immediate feedback for both sound and light.
- **Educational Modes:** Song guidance, scale learning, and custom patterns.
- **Portfolio-Ready:** Clean, modern codebase and documentation.

---

## Quick Start

### Hardware

- **Board:** ESP32-S3-DevKitC-1 (8MB/16MB flash, 8MB PSRAM supported)
- **LEDs:** WS2812B (Neopixel) strips
- **Buttons:** Robust pushbuttons, debounced in software

### Firmware

1. **Clone this repo**
2. **Install [PlatformIO](https://platformio.org/)**
3. **Connect ESP32-S3 via USB**
4. **Build and upload:**
   ```
   pio run -e esp32-s3-devkitc-1 -t upload --upload-port COMx
   ```
5. **Upload web assets to LittleFS:**
   ```
   pio run -e esp32-s3-devkitc-1 -t uploadfs --upload-port COMx
   ```

### Usage

- **WiFi AP:** Connect to `INT7-Piano` (password: `00000000`)
- **Web App:** Open [http://192.168.4.1](http://192.168.4.1) for real-time LED and pattern control
- **BLE MIDI:** Connect to `ESP32-Piano` from GarageBand or any BLE MIDI app

---

## Project Structure

```
├── src/
│   ├── main.cpp           # Main firmware (BLE MIDI, LED, button logic)
│   └── wifi_control.h     # WiFi AP, WebSocket, LittleFS web server
├── data/
│   ├── index.html         # Web app UI
│   ├── app.js             # Web app logic
│   └── style.css          # Web app styling
├── web_demo/              # (Optional) Standalone web demo
├── documentation/         # Schematics, BOM, hardware docs
├── platformio.ini         # PlatformIO project config
└── README.md              # This file
```

---

## Credits

**Developed by Team INT7:**
- Rafael Vardanyan, 3823239
- Steven Fokoua, 10001713
- Harshith Sadhu, 3824426
- Oduma Maxwell, 3823078
- Harith Binoy, 3823291
- Atharv Rajaram Kharoshe, 3824410

**Supervised by:**
- Prof. Dr.- Ing. Peter Burda
- Prof. Dr. Nicolas Tiesler

**Project developed on behalf of:**
- Mr. Paul Göttinger
- Prof. Dr.- Ing. Winfried Wilke

---

## License

This project is open-source under the MIT License.  
See [LICENSE](LICENSE) for details.

---

## Screenshots

![INT7 Modular Floor Piano](documentation/floor_piano_demo.jpg)

---

## Contact

For questions, collaboration, or feedback, please open an issue or contact the INT7 team.

---
