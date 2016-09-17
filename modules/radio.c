
#include <cassert>

#include "radio.h"


/*
 * Implementation notes:
 *
 * Interrupts are used.  Alternative is to poll (see RadioHead.)
 * Now, interrupt on receive.  FUTURE: interrupt on xmit.
 *
 * Uses "configuration registers" of the RADIO peripheral(section of the datasheet).
 * AND ALSO "task registers" of the "Peripheral Interface"(section of the datasheet)
 * to the RADIO (for tasks, events, and interrupts.)
 * See device/nrf52.h for definitions of register addresses.
 * Each task and event has its own register.
 *
 * Not keeping our own state (radio peripheral device has a state.)
 *
 * !!! Note this file has no knowledge of registers (nrf.h) , see radioLowLevel and radioConfigure.c
 */


// Class (singleton) data member
HfClock Radio::hfClock;



// Init: good even when radio is powered off.
void Radio::init() {
	enableDCDCPower();	// Radio device wants this enabled.
	// assert radio is configure to defaults.
	// ??? It is probably functional
	// But the code here is not set up to handle all default events from radio,
	// so additional configuration is required.
}




// Powering
// Power off saves more power than just disable.  A SoftDevice powers off to save power.

void Radio::powerOn() {
	// not require off; might be on already
	// require Vcc > 2.1V (see note below about DCDC)

	hfClock.start();	// radio requires XTAL!!! hf clock, not the RC hf clock
	setRadioPowered(true);
	disable();
	spinUntilDisabled();	// This is how RadioHead ensures radio is ready
	// ensure ready

	// assert if it was off, the radio and its registers are in initial state as specified by datasheet
	// i.e. it needs to be reconfigured

	// assert HFCLK is on since radio uses it
}


void Radio::powerOff() {
	// not require on; might be off already
	// not require disabled
	setRadioPowered(false);
	// not ensure not ready; caller must spin if necessary

	hfClock.stop();
	// assert hf RC clock resumes for other peripherals

	// assert radio and HFCLK are off, or will be soon
}


// Enabling

void Radio::enableRX() {
	clearEOTEvent();
	startRXTask();
}
void Radio::enableTX() {
	clearEOTEvent();
	startTXTask();
}

void Radio::disable() {
	clearDisabledEvent();
	startDisablingTask();
}



void Radio::spinUntilDisabled() {
	// Assert we start the task disable.
	// Wait until the event that signifies disable is complete
	// See data sheet.  For (1Mbit mode, TX) delay is ~6us, for RX, ~0us
	while (!isDisabled()) ;
}





// OBSOLETE?  Where did this come from?
// bool Radio::isReady() { return NRF_RADIO->EVENTS_READY; }




void Radio::setupXmitOrRcv(void * data) {
	// Setup common to xmit or rcv

	setShortcutsAvoidSomeEvents();

	passPacketAddress(data);


	enableInterruptForPacketDoneEvent();
	clearPacketDoneEvent();

	//NRF_RADIO->PREFIX1	= ((m_alt_aa >> 24) & 0x000000FF);
	//NRF_RADIO->BASE1    = ((m_alt_aa <<  8) & 0xFFFFFF00);
}

/*
 * In task/event architecture, these trigger or enable radio's tasks.
 *
 *
 */
void Radio::startXmit() {
	assert(isDisabled());  // require, else behaviour undefined per datasheet
	enableTX();
	// assert radio state will soon be TXRU
}

void Radio::startRcv() {
	assert(isDisabled());  // require, else behaviour undefined per datasheet
    enableRX();
	// assert radio state will soon be RXRU
}

void Radio::stopRcv() {
	// assert radio state is:
	// RXRU : aborting before ramp-up complete
	// or RXIDLE: on but never received start preamble signal
	// or RX: in middle of receiving a packet
	startDisablingTask();
	// assert radio state soon RXDISABLE and then immediately transitions to DISABLED
}

void Radio::stopXmit() {
	// Rarely used: to abort a transmission, generally radio completes a xmit
	// TODO
}

void Radio::spinUntilXmitComplete() {
	// Spin mcu until xmit complete.
	// Alternatively, sleep, but radio current dominates mcu current anyway?
	// Xmit takes a few msec?

	//assert isTransmitting

	// Nothing can prevent xmit?  Even bad configuration or HFCLK not on?

	// Radio state flows (via TXDISABLE) to DISABLED.  Wait for DISABLED event.
	// Here, for xmit, we do not enable interrupt on DISABLED event.
	// Since it we expect it to be quick.
	// TODO use interrupt on xmit.
	while (! isDisabled() ) {}
}




// Configuration

void Radio::configureAfterPowerOn() {
	// This should be redone whenever radio is power toggled on?
	// None of it may be necessary if you are happy with reset defaults?

	configureFixedFrequency();
	configureFixedAddress();
	configureCRC();

	//NRF_RADIO->PREFIX1	= ((m_alt_aa >> 24) & 0x000000FF);
	//NRF_RADIO->BASE1    = ((m_alt_aa <<  8) & 0xFFFFFF00);

	// FUTURE: parameter
	// Default tx power
};






