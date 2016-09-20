#pragma once

#include <inttypes.h>
#include "hfClock.h"
#include "radioDevice.h"


/*
 * High level driver for radio peripheral
 *
 * Understands collaboration of low-level devices:
 * - HfClock and RadioDevice
 * - DCDCPower and RadioDevice
 *
 * Not understand registers, i.e no dependence on nrf.h
 *
 * Understands wireless protocol:
 * - configures device for protocol
 * - abstracts protocol state from device state
 * - abstracts away the task/event architecture of device
 * - implements interrupt RX vs. spinning TX
 * - understands half-duplex (RX exclusive of TX)
 *
 * Singleton, all static class methods.
 */
class Radio {
private:
	static HfClock hfClock;
	static RadioDevice device;
	static bool wasTransmitting;  // false: wasReceiving
	static void (*aRcvMsgCallback)();


public:
	static void eventHandler();

	static void init(void (*onRcvMsgCallback)());
	static void configure();

	static void powerOn();
	static void powerOff();

	static bool isDisabled();

	static void transmit(uint8_t * data);
	static void receive(uint8_t * data);

	static void stopXmit();
	static void stopReceive();

	static void spinUntilXmitComplete();

private:
	static void dispatchPacketCallback();
	static void enableRX();
	static void enableTX();
	static void disable();
	static void spinUntilDisabled();
	static void setupXmitOrRcv(void * data);
	static void startXmit();
	static void startRcv();
};
