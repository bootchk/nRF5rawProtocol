

#include "ledFlasher.h"

#include "timer.h"
#include "ledLogger.h"


namespace {

Timer ledTimer;
LEDLogger ledLogger;

/*
 * Callback from timer, keep it short.
 * The timer interrupt wakes the mcu if was sleeping.
 */
void ledOffCallback(void* context) {
	ledLogger.switchLED(1, false);
}



} // namespace

void LEDFlasher::init() {
	ledTimer.create(ledOffCallback);
}

void LEDFlasher::flashLED(int ordinal) {
	flashLEDByAmount(ordinal, 1);
}

void LEDFlasher::flashLEDByAmount(int ordinal, int amount){
	// assert ledLogger initialized
	// assert TimerService initialized

	ledLogger.switchLED(ordinal, true);

	// start timer to turn LED off
	ledTimer.restartInUnitsTicks(amount * MinTicksPerFlash);
}
