#include <Arduino.h>
#include <FastLED.h>

// Arduino Uno Configuration
#define NUM_KEYS 3
#define LED_PIN 6          // PWM capable pin for LED data
#define BUTTON_PINS {2, 3, 4}  // Digital pins with interrupt capability
#define DEBOUNCE_DELAY 10  // ms

// Global variables
CRGB leds[NUM_KEYS];
uint8_t buttonPins[NUM_KEYS] = BUTTON_PINS;
bool buttonStates[NUM_KEYS] = {false};
unsigned long lastDebounceTimes[NUM_KEYS] = {0};

void setup() {
  Serial.begin(115200);
  Serial.println("Arduino Uno Piano Module - Basic Version");
  
  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_KEYS);
  FastLED.setBrightness(50);
  fill_solid(leds, NUM_KEYS, CRGB::Black);
  FastLED.show();

  // Initialize buttons with internal pullup
  for(int i = 0; i < NUM_KEYS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  // Check each button
  for(int i = 0; i < NUM_KEYS; i++) {
    bool currentState = digitalRead(buttonPins[i]);
    
    // Debounce check
    if(currentState != buttonStates[i]) {
      if(millis() - lastDebounceTimes[i] > DEBOUNCE_DELAY) {
        buttonStates[i] = currentState;
        lastDebounceTimes[i] = millis();
        
        // Button pressed (LOW due to pullup)
        if(!currentState) {
          // Set LED colors based on key index
          switch(i) {
            case 0:
              leds[i] = CRGB::Red;     // First white key
              break;
            case 1:
              leds[i] = CRGB::Blue;    // Second white key
              break;
            case 2:
              leds[i] = CRGB::Purple;  // Black key
              break;
          }
          
          Serial.print("Key ");
          Serial.print(i);
          Serial.println(" pressed");
        } else {
          // Turn off LED when button released
          leds[i] = CRGB::Black;
          
          Serial.print("Key ");
          Serial.print(i);
          Serial.println(" released");
        }
        
        FastLED.show();
      }
    }
  }
}
