
#include "ledLogger.h"

// see board.h for macros

// array of pin numbers of leds
constexpr uint8_t LEDLogger::leds_list[LEDS_NUMBER] = LEDS_LIST;

void LEDLogger::init(){
	// configure GPIO pins as digital out to LED
	LEDS_CONFIGURE(LEDS_MASK);
}

void LEDLogger::toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		LEDS_INVERT(1 << leds_list[i]);
		// nrf_delay_ms(500);
	}
}

void LEDLogger::toggleLED(int ordinal) {
	// no effect if ordinal out of range
	if ((ordinal < 1) || (ordinal > LEDS_NUMBER)) return;

	LEDS_INVERT(1 << leds_list[ordinal-1]);
}

