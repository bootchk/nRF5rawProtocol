
#pragma once


/*
 * Log to array of LED's.
 *
 * Safe to log to any index, they just won't light if they don't exist on the board as defined in boards.h
 *
 * Safe to have more than one instance.
 * E.G. a library can have its own instance,
 * but of course you must coordinate use of LEDs.
 */
class LEDLogger {
public:
	static void init();
	static void toggleLEDs();	// all
	static void toggleLED(int ordinal);
};
