#pragma once

/*
 * Flash an LED.
 *
 * Low power:
 * - just briefly enough to see
 * - use Timer to turn it off, so can sleep after calling flashLED
 */
class LEDFlasher {
public:
	/*
	 * Since we flash at sync points,
	 * we must not flash longer than a sync period,
	 * else LED still on next time we want to flash.
	 *
	 * Typical: sync period is 0.7 sec.
	 * So at .6mSec per flash, 1000 flashes almost fills the syncPeriod
	 *
	 */
	// This should be dependent on SyncPeriodDuration
	static const unsigned int MaxFlashAmount = 1000;

	/*
	 * Ticks for 32kHz clock are 0.030 mSec
	 * This gives a .6 mSec flash, which is barely visible in indoor room light.
	 */
	static const unsigned int MinTicksPerFlash = 20;

	static void init();

	/*
	 * No effect if ordinal out of range defined by boards.h
	 */

	/* Flash the minimal visible time. */
	static void flashLED(int ordinal);

	/*
	 * Flash at least minimal visible time, but less than MaxFlashAmount.
	 *
	 * Units are multiples of least visible time.
	 */
	static void flashLEDByAmount(int ordinal, int amount);
};
