#pragma once

#include <inttypes.h>
#include "hfClock.h"
#include "radioDevice.h"
#include "system.h"  // Nvic
#include "types.h"





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
 * Protocol is defined by:
 * - constants for lengths, channels, bitrate
 * - behaviour
 * -- no acks xmitted as in ESB
 * -- all units use one address
 *
 * Singleton, all static class methods.
 */
class Radio {
public:
	// Define protocol lengths
	// TODO payload up to 258 bytes?
	static const uint8_t PayloadCount = 5;
	static const uint8_t AddressLength = 4;	// 1 byte preamble, 3 bytes base

private:
	static HfClock hfClock;
	static RadioDevice device;
	static bool wasTransmitting;  // false: wasReceiving
	static Nvic nvic;
	static PowerSupply powerSupply;

	static void (*aRcvMsgCallback)();


public:
	static void eventHandler();

	static void init(void (*onRcvMsgCallback)());
	static void configure();

	static void powerOn();
	static void powerOff();
	static bool isPowerOn();

	static bool isDisabled();
	static bool isEnabledInterruptForPacketDoneEvent();

	static void transmit(BufferPointer data, uint8_t length);
	static void receive(BufferPointer data, uint8_t length);

	static void stopXmit();
	static void stopReceive();

	static void spinUntilXmitComplete();

private:
	static void dispatchPacketCallback();
	static void enableRX();
	static void enableTX();
	static void disable();
	static void spinUntilDisabled();
	static void setupXmitOrRcv(BufferPointer data, uint8_t length);
	static void startXmit();
	static void startRcv();

	static bool isValidPacket();

	// These mean EOTransmission, but currently only used for RX
	static bool isEventForEOTInterrupt();
	static void clearEventForEOTInterrupt();
	static void enableInterruptForEOT();
	static void disableInterruptForEOT();
public:
	static bool isEnabledInterruptForEOT();
};
