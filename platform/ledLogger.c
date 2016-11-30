
// #include "nrf_delay.h"	// For debugging

#include <cassert>
#include "ledLogger.h"

#include "boards.h"	// macros giving platform specific LED count, pin numbers


namespace {

// Sized and initialized from constants defined by macros
const uint8_t ledPinsArray[LEDS_NUMBER] = LEDS_LIST;

}	// namespace



void LEDLogger::init(){
	/*
	 * configure GPIO pins as digital out to LED.
	 * This references constants defined by macros, not ledPinsArray.
	 */
	LEDS_CONFIGURE(LEDS_MASK);

	LEDS_OFF(LEDS_MASK);

	// assert LED pins configured

	assert(! LED_IS_ON(LEDS_MASK));	// All LED pins in off state
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

#ifdef LED_LOGGING_TOGGLE
	LEDS_INVERT(1 << ledPinsArray[ordinal-1]);
#endif
}

void LEDLogger::switchLED(int ordinal, bool state) {
	// no effect if ordinal out of range
	if ((ordinal < 1) || (ordinal > LEDS_NUMBER)) return;

	uint32_t mask = 1 << ledPinsArray[ordinal-1];
	if (state)
		LEDS_ON(mask);
	else
		LEDS_OFF(mask);
}

