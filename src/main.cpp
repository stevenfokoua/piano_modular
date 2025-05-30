#include <Arduino.h>
#include <FastLED.h>

// Configuration
#define NUM_STRIPS 3
#define LEDS_PER_STRIP 3
#define TOTAL_LEDS (NUM_STRIPS * LEDS_PER_STRIP)
#define LED_PIN 6          // PWM capable pin for LED data
#define BUTTON_PINS {2, 3, 4}  // Digital pins with interrupt capability
#define DEBOUNCE_DELAY 10  // ms

// Global variables
CRGB leds[TOTAL_LEDS];
uint8_t buttonPins[NUM_STRIPS] = BUTTON_PINS;
bool buttonStates[NUM_STRIPS] = {false};
unsigned long lastDebounceTimes[NUM_STRIPS] = {0};

void setup() {
  Serial.begin(115200);
  Serial.println("Arduino Uno Piano Module - Basic Version");
  
  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  FastLED.setBrightness(50);
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();

  // Initialize buttons with internal pullup
  for(int i = 0; i < NUM_STRIPS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  // Check each button
  for(int i = 0; i < NUM_STRIPS; i++) {
    bool currentState = digitalRead(buttonPins[i]);
    
    // Debounce check
    if(currentState != buttonStates[i]) {
      if(millis() - lastDebounceTimes[i] > DEBOUNCE_DELAY) {
        buttonStates[i] = currentState;
        lastDebounceTimes[i] = millis();
        
        // Button pressed (LOW due to pullup)
        if(!currentState) {
          // Set LED colors based on Boomwhacker color pattern
          // C (Red), D (Orange), E (Yellow)
          CRGB color;
          switch(i) {
            case 0:
              color = CRGB(255, 0, 0);     // C - Red
              break;
            case 1:
              color = CRGB(255, 165, 0);   // D - Orange
              break;
            case 2:
              color = CRGB(255, 255, 0);   // E - Yellow
              break;
          }
          
          // Light up all LEDs in the corresponding strip
          int stripStart = i * LEDS_PER_STRIP;
          for(int j = 0; j < LEDS_PER_STRIP; j++) {
            leds[stripStart + j] = color;
          }
          
          Serial.print("Key ");
          Serial.print(i);
          Serial.println(" pressed");
        } else {
          // Turn off LED when button released
          // Turn off all LEDs in the corresponding strip
          int stripStart = i * LEDS_PER_STRIP;
          for(int j = 0; j < LEDS_PER_STRIP; j++) {
            leds[stripStart + j] = CRGB::Black;
          }
          
          Serial.print("Key ");
          Serial.print(i);
          Serial.println(" released");
        }
        
        FastLED.show();
      }
    }
  }
}
