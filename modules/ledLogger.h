
#include "boards.h"


class LEDLogger {
private:
	static const uint8_t leds_list[LEDS_NUMBER];

public:
	static void init();
	static void toggleLEDs();
	static void toggleLED(int ordinal);
};
