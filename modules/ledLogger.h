
#include "boards.h"	// defines LEDS_NUMBER and LEDS_LIST

/*
 * Log to set of LED's.
 * Safe to log to any index, they just won't light if they don't exist.
 */
class LEDLogger {
private:
	static const uint8_t leds_list[LEDS_NUMBER];

public:
	static void init();
	static void toggleLEDs();
	static void toggleLED(int ordinal);
};
