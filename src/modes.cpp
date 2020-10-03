#include "modes.h"
#include "leds.h"

namespace modes {

Mode current_mode;

Mode operator++(Mode& mode, int) {
  Mode tmp = mode;
  mode = static_cast<Mode>(
    (static_cast<int>(mode) + 1) % (static_cast<int>(Mode::Last) + 1)
  );
  return tmp;
}

void setup_current() {
  
  switch (current_mode) {
  case Mode::SPLIT_LOOP_5:
    split_loop::setup(12);
    Serial.println("split loop");
    break;
  case Mode::SEPERATED:
    seperated::setup();
    Serial.println("pulsing");
    break;
  case Mode::BOTTOM_UP:
    bottom_up::setup();
    Serial.println("bottom up");
    break;
  case Mode::ICE:
    ice::setup();
    Serial.println("ice");
    break;
  case Mode::STROBOPOP_WHITE:
    strobopop::setup(false);
    Serial.println("strobo");
    break;
  default:
      break;
  }
}

void setup() {
  current_mode = Mode::First;
  setup_current();
}

void cycle_mode() {
  current_mode++;
  setup_current();
}

void step_current() {
  switch (current_mode) {
  case Mode::SPLIT_LOOP_5:
    modes::split_loop::step();
    break;
  case Mode::SEPERATED:
    modes::seperated::step();
    break;
  case Mode::BOTTOM_UP:
    modes::bottom_up::step();
    break;
  case Mode::ICE:
    modes::ice::step();
    break;
  case Mode::STROBOPOP_WHITE:
    modes::strobopop::step();
    break;
  default:
    break;
  }
}

namespace split_loop {

static uint8_t splits;
static uint8_t leds_per_split;
static uint8_t hue_per_split;
static uint16_t rot;

void setup(uint8_t splits_) {
    splits = splits_;
    leds_per_split = NUM_LEDS / splits;
    hue_per_split = 256 / splits_;
    rot = 0;
}

void step() {
  for (uint8_t split = 0; split < splits; split++) {
    for (uint8_t i = 0; i < leds_per_split; i++) {
      uint8_t index = split * leds_per_split + i + (rot >> SPEED_SHIFT);
      setLED(index, split * hue_per_split, 255, brightness);
    }
  }
  FastLED.show();
  rot = (rot + 5 * ((speed >> 3) + 1)) % (NUM_LEDS << SPEED_SHIFT);
}

} // namespace split_loop


namespace bottom_up {

  static uint8_t color;
  static uint8_t progress = 0;

  void setup() {
    progress = 0;
  }

  void step() {
    uint8_t this_brightness = color < 3 ? brightness : 0;
    uint8_t this_hue = color * 80 + 10;
    uint8_t this_sat = 255;
    if (progress == 0 && color == 0) {
      setAllLED(0, 0, 0);
    }

    if (progress < 24) {
      setLED(progress, this_hue, this_sat, this_brightness);
    } else if (progress < 30) {
      // be slower in the middle
      static uint8_t counter = 0;
      counter++;
      if (counter % 3 != 0) {
        return;
      }
      uint8_t i = progress - 24;
      setLED(24 + i, this_hue, this_sat, this_brightness);
      setLED(35 - i, this_hue, this_sat, this_brightness);
      setLED(36 + i, this_hue, this_sat, this_brightness);
      setLED(47 - i, this_hue, this_sat, this_brightness);
    } else {
      setLED(progress + 18, this_hue, this_sat, this_brightness);
    }
    progress++;
    if (progress == 60) {
      progress = 0;
      color = (color + 1) % 4;
    }
    FastLED.show();
  }

} // namespace bottom_up


namespace seperated {

  static uint8_t splits = 3;
  static uint8_t leds_per_split = 4 * 6;
  static uint8_t hue_per_split = 256 / 3;
  static int16_t pulsing = 0;
  static uint8_t progress = 0;
  static bool up = false;

  void setup() {
    progress = 0;
  }

  void step() {
    if (progress == 0) {
      setAllLED(0, 0, 0);
    }
    if (progress < 66) {
      if (progress < 24) {
        setLED(progress, 0, 255, brightness);
      } else if (progress < 30) {
        // be slower in the middle
        static uint8_t counter = 0;
        counter++;
        if (counter % 3 != 0) {
          return;
        }
        uint8_t i = progress - 24;
        setLED(24 + i, hue_per_split, 255, brightness);
        setLED(35 - i, hue_per_split, 255, brightness);
        setLED(36 + i, hue_per_split, 255, brightness);
        setLED(47 - i, hue_per_split, 255, brightness);
      } else if (progress < 54) {
        setLED(progress + 18, 2 * hue_per_split, 255, brightness);
      }
      progress++;
      FastLED.show();
      return;
    }


    if (up) {
      pulsing += 4;
    } else {
      pulsing -= 4;
    }
    if (up && pulsing >= brightness - 10) {
      up = false;
      pulsing = brightness - 10;
    } else if (!up && pulsing <= 0) {
      up = true;
      pulsing = 0;
    }
    for (uint8_t split = 0; split < splits; split++) {
      for (uint8_t i = 0; i < leds_per_split; i++) {
        uint8_t index = split * leds_per_split + i;
        setLED(index, split * hue_per_split, 255, brightness - pulsing);
      }
    }
    FastLED.show();
    // rot = (rot + 5 * ((speed >> 3) + 1)) % (NUM_LEDS << SPEED_SHIFT);
  }

} // namespace seperated

namespace strobopop
{

static bool on;
static bool color;
static uint8_t hue;
static uint16_t progress;

void setup(bool color_) {
    on = false;
    color = color_;
    hue = 0;
    progress = DELAY - 2;
}

void step() {
    if (on) {
        if (progress == ON_TIME) {
          progress = 0;
          on = false;
          setAllLED(hue, 0, 0);
        } else {
            progress++;
        }
    } else {
        if (progress > DELAY) {
            progress = 0;
            on = true;
            uint8_t sat = color ? 127 : 0;
            setAllLED(hue, sat, brightness);
            if (color) {
                hue += millis();
            }
        } else {
            progress++;
        }
    }
    FastLED.show();
}

} // namespace strobopop

namespace ice
{

int16_t hue;
int16_t next_hue;
uint8_t leds_on_next_hue;
bool inc;

void setup() {
    hue = MIN_HUE;
    next_hue = hue + 1;
    inc = true;
    leds_on_next_hue = 0;
}

void step() {
    if (hue > MAX_HUE) {
        inc = false;
        hue = MAX_HUE;
        next_hue = hue - 1;
    } else if (hue < MIN_HUE) {
        inc = true;
        hue = MIN_HUE;
        next_hue = hue + 1;
    }

    uint16_t counter = 0;
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        counter += leds_on_next_hue;
        if (counter >= 10) {
            counter -= 10;
            setLED(i, next_hue, next_hue, brightness);
        } else {
            setLED(i, hue, hue, brightness);
        }
    }
    // for (uint8_t split = 0; split < 6; split++) {
    //     for (uint8_t i = 0; i < leds_on_next_hue; i++) {
    //         setLED(split * 10 + i, next_hue, next_hue, brightness);
    //     }
    //     for (uint8_t i = leds_on_next_hue; i < 10; i++) {
    //         setLED(split * 10 + i, hue, hue, brightness);
    //     }
    // }
    FastLED.show();

    leds_on_next_hue++;
    if (leds_on_next_hue == 10) {
      leds_on_next_hue = 0;
      if (inc) {
        hue++;
        next_hue = hue + 1;
      } else {
        hue--;
        next_hue = hue - 1;
      }
    }
}

} // namespace ice

} // namespace modes