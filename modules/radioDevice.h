
#pragma once

#include <inttypes.h>
#include "hfClock.h"


/*
 * Low-level driver for radio peripheral
 *
 * Understands registers, etc.
 * Does not understand any wireless protocol.
 * Provides services for wireless protocols.
 *
 * Singleton, all static class methods.
 */
class RadioDevice {
public:
	// Configuration: see notes in radioConfigure.c

	static void configureFixedFrequency();
	static void configureFixedLogicalAddress();
	static void configureNetworkAddressPool();
	static void configureCRC();
	static void configurePacketFormat();
	static void configureOnAirPacketFormat();
	static void configurePayloadFormat();

	static void configureWhiteningSeed(int);

	static void enableDCDCPower();
	static void passPacketAddress(void * data);

	static bool isCRCValid();

	static void setNetworkAddress(uint8_t* address, uint8_t len);

	// Tasks and events
	static void startRXTask();
	static void startTXTask();
	static bool isPacketDone();
	static void clearPacketDoneEvent();
	static void clearEOTEvent();

	static void startDisablingTask();
	static bool isDisabled();
	static void clearDisabledEvent();

	static void enableInterruptForPacketDoneEvent();
	static void setShortcutsAvoidSomeEvents();

	static void setRadioPowered(bool);
};
