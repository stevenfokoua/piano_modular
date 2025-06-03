# Hardware Setup

## ESP32-S3 DevKitC-1 Pinout
- **GPIO 2**: LED Data
- **GPIO 3**: Button 1
- **GPIO 4**: Button 2
- **GPIO 5**: Button 3
- **5V**: Power Supply
- **GND**: Ground

## Components
- ESP32-S3 DevKitC-1
- WS2812B LED Strip
- 3x Tactile Buttons
- 5V Power Supply
- 330Ω Resistor (for LED)
- 10kΩ Resistors (for buttons)

## Wiring Diagram
```mermaid
graph LR
    ESP32[ESP32-S3] -->|GPIO2| LED[LED Strip]
    ESP32 -->|GPIO3| BTN1[Button 1]
    ESP32 -->|GPIO4| BTN2[Button 2]
    ESP32 -->|GPIO5| BTN3[Button 3]
    ESP32 -->|5V| PWR[Power Supply]
    ESP32 -->|GND| GND[Ground]
