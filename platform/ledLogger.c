

// #include "nrf_delay.h"	// For debugging


#include "ledLogger.h"


// see board.h for macros

// array of pin numbers of leds
constexpr uint8_t LEDLogger::leds_list[LEDS_NUMBER] = LEDS_LIST;

void LEDLogger::init(){
	// configure GPIO pins as digital out to LED
	LEDS_CONFIGURE(LEDS_MASK);

	// Initial state not set, may depend on whether board was reset
}

// Toggle LEDs in order
// Commented out:  with 1 sec delay between
void LEDLogger::toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		toggleLED(i+1); // LEDS_INVERT(1 << leds_list[i]);
		//nrf_delay_ms(1000);
	}
}

void LEDLogger::toggleLED(int ordinal) {
	// no effect if ordinal out of range
	if ((ordinal < 1) || (ordinal > LEDS_NUMBER)) return;

	LEDS_INVERT(1 << leds_list[ordinal-1]);
}

