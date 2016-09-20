#pragma once

#include "radio.h"
#include "radioDevice.h"



/*
 * Transport layer for raw wireless protocol
 *
 * Owns and uses Radio driver.
 *
 * Singleton, all static class methods.
 */
class RawTransport {

public:
	static void eventHandler();

	static void init(void (*onRcvMsgCallback)());




	/*
	static void spinUntilXmitComplete();
	static void stopReceiver();

	static void powerOn();
	static void powerOff();

	static bool isDisabled();
	*/

private:
	static void dispatchPacketCallback();

	// static, class data members
	static Radio radio;
	static RadioDevice device;
	static void (*aRcvMsgCallback)();



};
