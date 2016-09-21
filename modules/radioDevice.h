
#pragma once

#include <inttypes.h>
#include "hfClock.h"
#include "types.h"


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
	static void configurePacketFormat(const uint8_t, const uint8_t );
	static void configureOnAirPacketFormat();
	static void configurePayloadFormat(const uint8_t, const uint8_t );

	// Getters of configuration
	static uint32_t frequency();

	static void configureWhiteningSeed(int);

	static void enableDCDCPower();
	static void passPacketAddress(BufferPointer data);

	static bool isCRCValid();

	static void setNetworkAddress(const uint8_t* address, const uint8_t len);

	// Tasks and events
	static void startRXTask();
	static void startTXTask();
	static bool isPacketDone();
	static void clearPacketDoneEvent();
	static void clearEOTEvent();

	static void startDisablingTask();
	static bool isDisabledEventSet();
	static void clearDisabledEvent();
	static bool isDisabled();

	static void enableInterruptForPacketDoneEvent();
	static void disableInterruptForPacketDoneEvent();
	static bool isEnabledInterruptForPacketDoneEvent();

	static void enableInterruptForDisabledEvent();
	static void disableInterruptForDisabledEvent();
	static bool isEnabledInterruptForDisabledEvent();

	static void setShortcutsAvoidSomeEvents();

	static void setRadioPowered(bool);
	static bool isPowerOn();
};
