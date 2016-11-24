
#pragma once


/*
 * Log to array of LED's.
 *
 * Safe to log to any index, they just won't light if they don't exist on the board as defined in boards.h
 *
 * Safe to have more than one instance.
 * E.G. a library can have its own instance,
 * but of course you must coordinate use of LEDs.
 *
 * Toggle: LED's stay on/off until the next call,
 * so they indicate state of debugging by their transitions.
 *
 * !!! This is not thread safe: it doesn't test that pins remain configured as outputs.
 * I assume that writing to a pin that is configured as input is harmless.
 *
 * You must call init() to guarantee pins are configured properly:
 * Valid sequence is: init(), toggleLEDs(), ...
 */
class LEDLogger {
public:
	static void init();
	static void toggleLEDs();	// all
	static void toggleLED(int ordinal);
	static void switchLED(int ordinal, bool state);
};
