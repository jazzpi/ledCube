#include <stdint.h>

namespace modes {

enum class Mode {
    SPLIT_LOOP_5, SEPERATED, ICE, STROBOPOP_WHITE, STROBOPOP_COL,
    First = SEPERATED,
    Last = STROBOPOP_COL
};

extern Mode current_mode;

void setup();
void cycle_mode();
void step_current();

namespace split_loop {
void setup(uint8_t splits);
void step();
} // namespace split_loop

namespace seperated {
    void setup();
    void step();
} // namespace seperated

namespace strobopop
{
constexpr uint16_t ON_TIME = 1;
constexpr uint16_t DELAY = 32;

void setup(bool color);
void step();
} // namespace strobopop

namespace ice {
constexpr int16_t MIN_HUE = 125;
constexpr int16_t MAX_HUE = 170;

void setup();
void step();
} // namespace ice

} // namespace modes
