
#pragma once

#include <inttypes.h>


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
	// 32/khz divide by 17 is about 1ms per tick
	// 16 = 2^5
	// 2^8-1 yields 125ms per tick
	static const uint32_t TimerPrescaler = 0;

	// Only two timers, but +1 ???  See tutorial.
	static const uint8_t TimerQueueSize = 3;

	static const uint32_t MaxTimeout = 0xFFFFFF;	// 24-bit

	static bool isPlaceholderStarted;


	public:
		static void init();
		static void createTimers(void (*func)(void*));
		// Units mSec
		static void restartInMSec(int timeout);
		// Units OSTicks i.e. resolution of RTC1 counter
		static void restartInTicks(uint32_t timeout);

		static void startPlaceholder();
		static bool isOSClockRunning();

	private:
		static void createOneShot(void (*func)(void*));
		static void createPlaceholderTimer();

};
