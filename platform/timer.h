
#pragma once

#include <inttypes.h>


/*
 * Thin wrapper around NRF SDK app_timer.
 *
 * Uses 32khz xtal clock.
 * 
 */
class Timer {
private:
	// 32/khz divide by 17 is about 1ms per tick
	static const uint32_t TimerPrescaler = 16;

	// Only two timer, but +1 ???  See tutorial.
	static const uint8_t TimerQueueSize = 3;

	static const uint32_t MaxTimeout = 0xFFFFFF;	// 24-bit

	public:
		static void init();
		static void createTimers(void (*func)(void*));
		static void restart(int timeout);
	private:
		static void createOneShot(void (*func)(void*));
		static void createPlaceholderTimer();

		static void startPlaceholder();

};
