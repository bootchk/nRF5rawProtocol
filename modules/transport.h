#pragma once

#include "radio.h"



/*
 * Transport layer for raw wireless protocol
 *
 * Owns and uses Radio driver.
 *
 * Singleton, all static class methods.
 */
class RawTransport {

public:
	// use in ISR vector table, not call directly
	static void eventHandler();

	static void configure();
	static void transmit(void * data);

	static void startReceiver();
	static void stopReceiver();

	static void powerOn();
	static void powerOff();

	static bool isDisabled();

private:
	static void dispatchPacketCallback();

	// static, class data members
	static Radio radio;

};
