
#include "boards.h"	// via nested includes, depends on -DBOARD_<FOO> , defines LEDS_NUMBER and LEDS_LIST

/*
 * Log to set of LED's.
 *
 * Safe to log to any index, they just won't light if they don't exist.
 *
 * Safe to have more than one instance.
 * E.G. a library can have its own instance,
 * but of course you must coordinate use of LEDs.
 */
class LEDLogger {
private:
	static const uint8_t leds_list[LEDS_NUMBER];

public:
	static void init();
	static void toggleLEDs();
	static void toggleLED(int ordinal);
};
