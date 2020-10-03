#include "controls.h"
#include "modes.h"

#include "Arduino.h"
#include "config.h"
#include "leds.h"

int btn_interrupt = 0;
bool on = false;


void controls_setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEED_PIN, INPUT);
  pinMode(BRIGHTNESS_PIN, INPUT);

  // Interrupt setup, see https://playground.arduino.cc/Main/PinChangeInterrupt/
  // Enable pin
  // *digitalPinToPCMSK(BUTTON_PIN) |= bit(digitalPinToPCMSKbit(BUTTON_PIN));
  // Clear any outstanding interrupt
  // PCIFR |= bit(digitalPinToPCICRbit(BUTTON_PIN));
  // // Enable interrupt for the group
  // PCICR |= bit(digitalPinToPCICRbit(BUTTON_PIN));
}

int btn_click() {
  static uint8_t last_val = 0;
  static unsigned long last_time = 0;
  unsigned long time = millis();
  if (time - last_time < 10 && last_val == 0) {
    return 0;
  }
  last_time = time;

  uint8_t val = digitalRead(BUTTON_PIN);
  if (val == 0 && last_val == 1) {
    last_val = val;
    return 1;
  }
  last_val = val;
  return 0;
}

// #if 0 <= BUTTON_PIN && BUTTON_PIN <= 7
// // ISR(PCINT2_vect)
// // #elif 8 <= BUTTON_PIN && BUTTON_PIN <= 13
// // ISR(PCINT0_vect)
// // #elif A0 <= BUTTON_PIN && BUTTON_PIN <= A5
// ISR(PCINT1_vect)
// #else
// #error "Invalid BUTTON_PIN"
// #endif
// {
//   unsigned long time = millis();
//   if (time - last_time < 50 || btn_interrupt) return;
//   uint8_t val = digitalRead(BUTTON_PIN);
//   Serial.println(val);
//   last_val = val;
//   if (val == 0 && last_val == 1) {
//     last_time = time;
//     btn_interrupt = 1;
//   }
// }

inline uint16_t shift_right_round(uint16_t val, uint8_t shift) {
  uint16_t ret = val >> shift;
  if (val & (1 << (shift - 1))) {
    // Round up
    ret++;
  }
  return ret;
}

void controls_step() {
  on = (digitalRead(SWITCH_PIN) == LOW) ? true : false;


  if (btn_click()) {
    modes::cycle_mode();
  }

  speed = shift_right_round(1023 - analogRead(SPEED_PIN), SPEED_SHIFT);
  uint16_t value = shift_right_round(analogRead(BRIGHTNESS_PIN), BRIGHTNESS_SHIFT);
  brightness = value + MIN_BRIGHTNESS;
  if (brightness < value) {
    brightness = 255;
  }
  // Serial.print(value);
  // Serial.print(", ");
  // Serial.println(brightness);
  // brightness = 60;
}