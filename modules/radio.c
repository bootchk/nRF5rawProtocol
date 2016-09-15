
#include <cassert>

#include "nrf.h"

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
 */


// Class (singleton) data member
HfClock Radio::hfClock;


// Private
static void clearEOTEvent() {
	// end of transceive (transmission or reception) will set this event
	// The radio state becoming "disabled" signifies EOT
	NRF_RADIO->EVENTS_DISABLED = 0U;	// Clear event register
}

static void setRXTask() {
	NRF_RADIO->TASKS_RXEN = 1;	// write TASK register
}
static void setTXTask() {
	NRF_RADIO->TASKS_TXEN = 1;	// write TASK register
}



// Init: good even when radio is powered off.
void Radio::init() {
	// Not really the radio, another peripheral.
	// Per Radiohead
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 0x00000001;
}


// Powering
// Power off saves more power than just disable.  A SoftDevice powers off to save power.

void Radio::powerOn() {
	// not require off; might be on already
	// require Vcc > 2.1V (see note below about DCDC)

	hfClock.start();	// radio requires XTAL!!! hf clock, not the RC hf clock

	NRF_RADIO->POWER = 1;	// per datasheet

	// OBSOLETE?  Not really sure how to know you can start writing to registers
	// while ( ! isReady() ) {};	// spin until radio ramps up

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
	NRF_RADIO->POWER = 0;
	// not ensure not ready; caller must spin if necessary

	hfClock.stop();
	// assert hf RC clock resumes for other peripherals

	// assert radio and HFCLK are off, or will be soon
}


// Enabling

void Radio::enableRX() {
	clearEOTEvent();
	setRXTask();
}
void Radio::enableTX() {
	clearEOTEvent();
	setTXTask();
}

void Radio::disable() {
	// From RadioHead
	NRF_RADIO->EVENTS_DISABLED = 0;
	NRF_RADIO->TASKS_DISABLE = 1;
}

bool Radio::isDisabled() {
	// i.e. not busy with (in midst of) xmit or rcv
	return NRF_RADIO->EVENTS_DISABLED; // == 1;
}

void Radio::spinUntilDisabled() {
	// Assert we set the task disable.
	// Wait until the event that signifies disable is complete
	// See data sheet.  For (1Mbit mode, TX) delay is ~6us, for RX, ~0us
	while (!isDisabled()) ;
}





// OBSOLETE?  Where did this come from?
// bool Radio::isReady() { return NRF_RADIO->EVENTS_READY; }



// States




bool Radio::isPacketDone() {
	// packet was received.
	// radio state usually RXIDLE or TXIDLE and
	return NRF_RADIO->EVENTS_END;  // == 1;
}

void Radio::clearPacketDoneFlag() {
	NRF_RADIO->EVENTS_END = 0;
}

bool Radio::isCRCValid() {
	return NRF_RADIO->CRCSTATUS == 1;	// CRCOk;
}




void Radio::setupXmitOrRcv(void * data) {
	// Setup common to xmit or rcv

	// setup shortcuts, i.e. avoid some events
	NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;

	// point to packet in memory, must fit in 4 byte register
	NRF_RADIO->PACKETPTR = (uint32_t) data;

	// enable interrupt for 'packet done'
	NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;

	// clear packet done flag
	NRF_RADIO->EVENTS_END = 0;
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
	NRF_RADIO->TASKS_DISABLE = 1;
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

	// Radio state flows (via TXDISABLE) to DISABLED.  Wait for DISABLED event flagged.
	// There is no interrupt enabled on DISABLED event.
	while (NRF_RADIO->EVENTS_DISABLED == 0U) {}
}




// Configuration

void Radio::configureAfterPowerOn() {
	// This should be redone whenever radio is power toggled on?
	// None of it may be necessary if you are happy with reset defaults?

	setFixedFrequency();
	setFixedAddress();
	setCRC();

	//NRF_RADIO->PREFIX1	= ((m_alt_aa >> 24) & 0x000000FF);
	//NRF_RADIO->BASE1    = ((m_alt_aa <<  8) & 0xFFFFFF00);

	// FUTURE: parameter
	// Default tx power
};


void Radio::setFixedFrequency(){
	// FUTURE: parameter
	// Set Freq fixed to one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Whitening: helpful side effect.
	// Whitening depends on freq?
	NRF_RADIO->DATAWHITEIV = 37 & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;	// or 38, 39
}

/*
 * Receive same address we transmit. (But we don't receive our own transmissions.)
 */
void Radio::setFixedAddress(){
	// FUTURE: parameter

	// Physical on-air address is set in PREFIX0 + BASE0 by setNetworkAddress
	// Not require it to be set, the default should work?

	NRF_RADIO->TXADDRESS   = 0x00;	// Transmit to logical address 0 (PREFIX0 + BASE0)
	NRF_RADIO->RXADDRESSES = 0x01; // Receive logical address 0 (PREFIX0 + BASE0)
	// There are many RX addresses.  We only chose one.
	// FUTURE setLogicalAddresses()
}


void Radio::setCRC() {
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
	NRF_RADIO->CRCPOLY = 0x11021UL; // CRC poly: x^16+x^12^x^5+1
}

