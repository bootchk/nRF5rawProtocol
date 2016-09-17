#pragma once

#include "hfClock.h"


/*
 * Driver for radio peripheral
 *
 * Understands registers, etc.
 * Does not understand any wireless protocol.
 * Provides services for wireless protocols.
 *
 * Singleton, all static class methods.
 */
class Radio {
private:
	static HfClock hfClock;

public:
	static void init();
	static void powerOn();
	static void powerOff();

	static void enableRX();
	static void enableTX();
	static void disable();
	static bool isDisabled();
	static void spinUntilDisabled();

	//static bool isReady();

	static bool isPacketDone();

	static bool isCRCValid();

	static void setupXmitOrRcv(void * data);
	static void startXmit();
	static void startRcv();
	static void stopXmit();
	static void stopRcv();
	static void spinUntilXmitComplete();

	// Configuration
	static void configureAfterPowerOn();

private:
	static void enableDCDCPower();
	static void passPacketAddress(void * data);

	static void configureFixedFrequency();
	static void configureFixedAddress();
	static void configureCRC();

	// Tasks and events
	static void clearEOTEvent();
	static void startRXTask();
	static void startTXTask();
	static void startDisablingTask();
	static void clearDisabledEvent();
	static void enableInterruptForPacketDoneEvent();
	static void setShortcutsAvoidSomeEvents();
	static void setRadioPowered(bool);

public:
	static void clearPacketDoneEvent();


};
