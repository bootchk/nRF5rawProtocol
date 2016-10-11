
#pragma once

#include <inttypes.h>


/*
 * Free-running counter.
 *
 * "OS" clock, but the os is just a set of libraries on the platform, nothing sophisticated.
 */
/*
 * 24-bits of free-running counter, in LSB of 32-bit word
 *
 * Since RTC is started/stopped by app_timer, must keep a Timer scheduled
 * to ensure app_timer does not stop RTC.
 *
 * Roll over handled by library wedge.
 *
 * app_timer uses RTC1
 * SoftDevice uses RTC0
 * nrf52 has additional RTC2
 *
 * To save power, use RTC1 shared with app_timer
 */

typedef uint32_t OSTime;
// Alias for OSTime: time that platform accepts for timeouts, scheduling
typedef uint32_t DeltaTime;
// NRF accepts 32 bits but only 24-bits valid

// The size of the OSClock in bits.  Used in LongClock.
const uint8_t OSClockCountBits = 24;
const uint32_t MaxDeltaTime = 0xFFFFFF;	// 24-bits



class OSClock {
public:
	// This could be implemented on app_timer,
	// but apparently not directly on nrf drivers
	// a bit of a hack and not really needed.
	// FUTURE static bool isRunning();

	/*
	 * Now time of platform's free-running, circular clock.
	 *
	 * Choosing the platform's more accurate clock is better for synchronization algorithm.
	 */
	static OSTime ticks();
};
