#include "leds.h"

#include "config.h"

CRGB leds[NUM_LEDS];
uint8_t brightness;
uint8_t speed;

void led_setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(LED_PIN, OUTPUT);

  brightness = 0;
  speed = 0;
}

void led_off() {
  FastLED.clear(true);
}

void setAllLED(uint8_t hue, uint8_t sat, uint8_t val) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    setLED(i, hue, sat, val);
  }
}