
#include "ledLogger.h"

// array of pin numbers of leds
constexpr uint8_t LEDLogger::leds_list[LEDS_NUMBER] = LEDS_LIST;

void LEDLogger::init(){
	LEDS_CONFIGURE(LEDS_MASK);
}

void LEDLogger::toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		LEDS_INVERT(1 << leds_list[i]);
		// nrf_delay_ms(500);
	}
}

void LEDLogger::toggleLED(int ordinal) { LEDS_INVERT(1 << leds_list[ordinal-1]); }

