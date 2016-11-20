
#pragma once

#include <inttypes.h>

#include "osClock.h"  // OSTime

/*
 * Thin wrapper around NRF SDK app_timer.
 *
 * Uses 32khz xtal clock.
 * 
 * Creates two timers:
 * - a single-shot that is exported and can be restarted
 * - a placeholder that does nothing and is mostly not exported, except it starts OSClock
 *
 */
class Timer {
private:


	static bool isPlaceholderStarted;


public:
	//static void init();
	static void createTimers(void (*func)(void*));
	// Units mSec
	static void restartInMSec(int timeout);
	// Units OSTicks i.e. resolution of RTC1 counter
	static void restartInTicks(OSTime timeout);
	static void cancelTimeout();	// Cancel oneShot

	static void startPlaceholder();
	static bool isOSClockRunning();

private:
	static void createOneShot(void (*func)(void*));
	static void createPlaceholderTimer();

};
