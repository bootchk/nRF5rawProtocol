

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
	// assert ledLogger initialized
	// assert TimerService initialized

	ledLogger.switchLED(ordinal, true);

	// startTimerToOff
	ledTimer.restartInUnitsTicks(20);	// 20 ticks is 0.6 mSec
}
