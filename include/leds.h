#include "FastLED.h"

#define NUM_LEDS 72
#define SPEED_SHIFT 4
#define MIN_BRIGHTNESS 16
// #define MAX_BRIGHTNESS 100
#define BRIGHTNESS_SHIFT 2

extern CRGB leds[NUM_LEDS];
extern uint8_t brightness;
extern uint8_t speed;

inline void setLED(uint8_t i, uint8_t hue, uint8_t sat, uint8_t val) {
  leds[i % NUM_LEDS] = CHSV(hue, sat, val);
}

void led_setup();
void led_off();
void setAllLED(uint8_t hue, uint8_t sat, uint8_t val);