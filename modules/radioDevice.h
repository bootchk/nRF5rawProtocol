
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
	static void configureStaticPacketFormat(const uint8_t, const uint8_t );
private:
	static void configureCRCPolynomialForShortData();
	static void configureStaticOnAirPacketFormat();
	static void configureStaticPayloadFormat(const uint8_t PayloadCount, const uint8_t AddressLength);
	static void configureWhiteningSeed(int);

	static void setFirstNetworkAddressInPool(const uint8_t* address, const uint8_t len);

public:
	static void configurePacketAddress(BufferPointer data);
	static void configurePacketLengthDynamic(uint8_t length);



	// Getters of configuration
	static uint32_t frequency();


	static bool isCRCValid();

	// Tasks and events
	static void startRXTask();
	static void startTXTask();
#ifdef USE_PACKET_DONE_FOR_EOT
	static bool isPacketDone();
	static void clearPacketDoneEvent();
#else
	// Not implemented or used static bool isEOTEvent();
	static void clearMsgReceivedEvent();
	static void clearEndTransmitEvent();
#endif

	// !!!
	static void startDisablingTask();
	static bool isDisabledEventSet();
	static void clearDisabledEvent();
	static bool isDisabled();

#ifdef USE_PACKET_DONE_FOR_EOT
	static void enableInterruptForPacketDoneEvent();
	static void disableInterruptForPacketDoneEvent();
	static bool isEnabledInterruptForPacketDoneEvent();
#else
	// use disabled event for EOT
	static void enableInterruptForDisabledEvent();
	static void disableInterruptForDisabledEvent();
	static bool isEnabledInterruptForDisabledEvent();
#endif

	static void setShortcutsAvoidSomeEvents();

	static void setRadioPowered(bool);
	static bool isPowerOn();
};
