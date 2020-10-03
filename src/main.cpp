#include <Arduino.h>

#include "leds.h"
#include "modes.h"
#include "config.h"
#include "controls.h"

void reset() {
  led_setup();
  led_off();
  controls_setup();
  modes::setup();
}

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  reset();
}

static unsigned counter = 0;
unsigned long last_time;

void loop() {
  controls_step();

  if (!on) {
    reset();
    return;
  }

  if (counter++ % ((64 - speed) * 4 + 1) == 0) {
    modes::step_current();
  }

  // delay(1);
  // delay(30);
}