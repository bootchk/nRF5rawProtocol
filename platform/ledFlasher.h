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
	static void init();

	/*
	 * No effect if ordinal out of range defined by boards.h
	 */
	static void flashLED(int ordinal);
};
