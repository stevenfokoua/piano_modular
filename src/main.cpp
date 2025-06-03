#include <Arduino.h>
#include <FastLED.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

// Standard MIDI Service UUID
#define MIDI_SERVICE_UUID "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"
#define MIDI_CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3"

// Debug flag
#define DEBUG_OUTPUT 1

// Configuration
#define ONBOARD_LED_PIN 48
#define EXTERNAL_LED_PIN 4
#define NUM_EXTERNAL_LEDS 12  // Four LEDs per button (3 buttons × 4 LEDs)
#define LEDS_PER_BUTTON 4    // Number of LEDs that light up per button press
#define ONBOARD_BRIGHTNESS 32
#define EXTERNAL_BRIGHTNESS 255

// Button Configuration
#define NUM_BUTTONS 3
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {5, 6, 7};  // GPIO pins for buttons
#define DEBOUNCE_TIME 50

// MIDI Configuration
#define MIDI_CHANNEL 0  // MIDI channels are 0-15 (displayed as 1-16)
const uint8_t MIDI_NOTES[NUM_BUTTONS] = {60, 62, 64};  // C4, D4, E4

// LED Colors (Boomwhacker colors)
const CRGB NOTE_COLORS[NUM_BUTTONS] = {
    CRGB::Red,     // C - Red
    CRGB::Orange,  // D - Orange
    CRGB::Yellow   // E - Yellow
};

// Global variables
CRGB onboard_led;
CRGB external_leds[NUM_EXTERNAL_LEDS];
bool button_states[NUM_BUTTONS] = {false};
unsigned long last_debounce_time[NUM_BUTTONS] = {0};

// BLE MIDI variables
NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;
bool notificationsEnabled = false;

class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
        notificationsEnabled = subValue == 1;
        Serial.printf("Notifications %s\n", notificationsEnabled ? "enabled" : "disabled");
    }
};

// Forward declarations
void handleButton(uint8_t button);
void updateLEDs();
void sendMidiMessage(uint8_t status, uint8_t data1, uint8_t data2);
void sendMidiNote(uint8_t note, bool on);

class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Client connected");
    };

    void onDisconnect(NimBLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Client disconnected");
        // Restart advertising
        pServer->startAdvertising();
    }
};

void sendMidiMessage(uint8_t status, uint8_t data1, uint8_t data2) {
    if (deviceConnected && notificationsEnabled) {
        uint8_t midiPacket[] = {
            0x80,  // Header
            0x80,  // Timestamp
            status,  // Status byte
            data1,   // Data 1
            data2    // Data 2
        };
        pCharacteristic->setValue(midiPacket, 5);
        pCharacteristic->notify();
        if(DEBUG_OUTPUT) {
            Serial.printf("MIDI message sent: status=%02X, data1=%02X, data2=%02X\n", 
                status, data1, data2);
        }
    } else if(DEBUG_OUTPUT) {
        Serial.printf("Cannot send MIDI: connected=%d, notifications=%d\n", 
            deviceConnected, notificationsEnabled);
    }
}

void sendMidiNote(uint8_t note, bool on) {
    if (on) {
        sendMidiMessage(0x90 | MIDI_CHANNEL, note, 127);  // Note On, full velocity
    } else {
        sendMidiMessage(0x80 | MIDI_CHANNEL, note, 0);    // Note Off
    }
}

void handleButton(uint8_t button) {
    if(button_states[button]) {
        // Button pressed - light up all LEDs for this button
        for(int i = 0; i < LEDS_PER_BUTTON; i++) {
            external_leds[button * LEDS_PER_BUTTON + i] = NOTE_COLORS[button];
        }
        sendMidiNote(MIDI_NOTES[button], true);
        Serial.printf("Button %d pressed - Note On: %d\n", button, MIDI_NOTES[button]);
    } else {
        // Button released - turn off all LEDs for this button
        for(int i = 0; i < LEDS_PER_BUTTON; i++) {
            external_leds[button * LEDS_PER_BUTTON + i] = CRGB::Black;
        }
        sendMidiNote(MIDI_NOTES[button], false);
        Serial.printf("Button %d released - Note Off: %d\n", button, MIDI_NOTES[button]);
    }
}

void updateLEDs() {
    // Status LED pattern
    static unsigned long last_blink = 0;
    static bool led_state = true;
    
    // Blink onboard LED based on connection and button state
    bool any_button_pressed = false;
    for(int i = 0; i < NUM_BUTTONS; i++) {
        if(button_states[i]) {
            any_button_pressed = true;
            break;
        }
    }
    
    if(!deviceConnected) {
        // Slow blink when not connected
        if(millis() - last_blink > 1000) {
            led_state = !led_state;
            onboard_led = led_state ? CRGB::Blue : CRGB::Black;
            last_blink = millis();
        }
    } else if(any_button_pressed) {
        // Fast blink when connected and active
        if(millis() - last_blink > 100) {
            led_state = !led_state;
            onboard_led = led_state ? CRGB::Green : CRGB::Black;
            last_blink = millis();
        }
    } else {
        // Solid when connected but idle
        onboard_led = CRGB::Green;
    }
    
    FastLED.show();
}

void setup() {
    Serial.begin(115200);
    
    // Initialize BLE
    NimBLEDevice::init("ESP32-Piano");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    // Create MIDI service and characteristic
    NimBLEService* pService = pServer->createService(MIDI_SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        MIDI_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE |
        NIMBLE_PROPERTY::WRITE_NR |
        NIMBLE_PROPERTY::NOTIFY |
        NIMBLE_PROPERTY::INDICATE
    );
    
    // Set characteristic callbacks
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    
    // Start the service
    pService->start();
    
    // Start advertising
    NimBLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(MIDI_SERVICE_UUID);
    pAdvertising->start();
    
    Serial.println("BLE MIDI device advertising...");
    
    // Initialize LEDs
    FastLED.addLeds<WS2812B, ONBOARD_LED_PIN, GRB>(&onboard_led, 1);
    FastLED.addLeds<WS2812B, EXTERNAL_LED_PIN, GRB>(external_leds, NUM_EXTERNAL_LEDS);
    FastLED.setBrightness(EXTERNAL_BRIGHTNESS);
    FastLED.clear(true);  // Clear all LEDs
    
    // Initialize buttons with internal pullup
    for(int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }
    
    // Initial LED state
    onboard_led = CRGB::Blue;
    FastLED.show();
    if(DEBUG_OUTPUT) {
        Serial.println("LEDs initialized");
    }
    Serial.println("ESP32-S3 Piano v0.4 Initialized");
}

void loop() {
    static bool last_reading[NUM_BUTTONS] = {false};
    static bool button_stable[NUM_BUTTONS] = {true};
    
    // Check each button
    for(int i = 0; i < NUM_BUTTONS; i++) {
        bool reading = !digitalRead(BUTTON_PINS[i]);  // Invert because of pullup
        
        // If the reading has changed, reset the debounce timer
        if(reading != last_reading[i]) {
            last_debounce_time[i] = millis();
            button_stable[i] = false;
        }
        
        // If we have a stable reading for the debounce period
        if(!button_stable[i] && ((millis() - last_debounce_time[i]) > DEBOUNCE_TIME)) {
            // Reading is stable
            button_stable[i] = true;
            if(reading != button_states[i]) {
                button_states[i] = reading;
                handleButton(i);
            }
        }
        
        last_reading[i] = reading;
    }
    
    // Update LEDs based on button states
    updateLEDs();
    
    // Small delay to prevent button reading from being too aggressive
    delay(1);
}
