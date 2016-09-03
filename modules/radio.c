
#include <cassert>

#include "nrf.h"

#include "radio.h"



// Powering

void Radio::powerOn() {
	// not require off; might be on already
	NRF_RADIO->POWER = 1;
	// spin until radio ramps up
	while ( ! isReady() ) {};
	// ensure ready
}

void Radio::powerOff() {
	// not require on; might be off already
	// not require disabled
	NRF_RADIO->POWER = 0;
	// not ensure not ready; caller must spin if necessary
}

bool Radio::isReady() { return NRF_RADIO->EVENTS_READY; }



// States

bool Radio::isDisabled() {
	// i.e. not busy with (in midst of) xmit or rcv
	return NRF_RADIO->EVENTS_DISABLED; // == 1;
}

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
 * Not keeping our own state (radio peripheral device has a state.)
 */
void Radio::startXmit() {
	assert(isDisabled);  // require, else behaviour undefined per datasheet
	NRF_RADIO->TASKS_TXEN = 1;
	// assert radio state will soon be TXRU
}

void Radio::startRcv() {
	assert(isDisabled);  // require, else behaviour undefined per datasheet
    NRF_RADIO->TASKS_RXEN = 1;
	// assert radio state will soon be RXRU
}




// Configuration

void Radio::setFixedFrequency(){
	// FUTURE: parameter
	// Set Freq fixed to one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Whitening: helpful side effect.
	// Whitening depends on freq?
	NRF_RADIO->DATAWHITEIV = 37 & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;	// or 38, 39
}

void Radio::setFixedAddress(){
	// FUTURE: parameter

	// Receive same address we transmit. (But we don't receive our own transmissions.)
	// Receive only logical address 0 (or 1?) i.e. address configured by
	NRF_RADIO->RXADDRESSES = 0x01;

	// TODO Choose fixed logical address
	// TODO setLogicalAddresses
}


