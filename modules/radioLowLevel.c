
#include "nrf.h"

#include "radio.h"

// Low-level, mostly private
// These are here so that most knowledge of a kind of register manipulation is in one place.
// We don't use comments at a higher level to explain register manipulation, the name suffices.

/*
 * Clear means: clear the flag that indicates an event.
 * You start a task, which runs in the radio device.
 * The task eventually sets an event.
 * The event can trigger an interrupt.
 */


// TODO inline

void Radio::enableDCDCPower(){
	// Not really the radio, another peripheral.
	// Per Radiohead
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 0x00000001;
}

void Radio::passPacketAddress(void * data){
	// point to packet in memory, must fit in 4 byte register
	// Is portable until address space exceeds 32-bit
	NRF_RADIO->PACKETPTR = (uint32_t) data;
}


// Events and tasks


void Radio::clearEOTEvent() {
	// end of transceive (transmission or reception) will set this event
	// The radio state becoming "disabled" signifies EOT
	NRF_RADIO->EVENTS_DISABLED = 0U;	// Clear event register
}

void Radio::startRXTask() {
	NRF_RADIO->TASKS_RXEN = 1;	// write TASK register
}
void Radio::startTXTask() {
	NRF_RADIO->TASKS_TXEN = 1;	// write TASK register
}

void Radio::startDisablingTask(){
	NRF_RADIO->TASKS_DISABLE = 1;
}

void Radio::clearDisabledEvent(){
	NRF_RADIO->EVENTS_DISABLED = 0;
}


void Radio::enableInterruptForPacketDoneEvent() {
	NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;
}

void Radio::setShortcutsAvoidSomeEvents() {
	// The radio CAN emit events for may state transitions we are not interested in.
	// TODO Avoid READY,  event,
	NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
}

void Radio::setRadioPowered(bool value){
	NRF_RADIO->POWER = value;	// 1 or 0 per C++ standard and datasheet
}


// public

// states

bool Radio::isPacketDone() {
	// packet was received.
	// radio state usually RXIDLE or TXIDLE and ???
	// Event "END" means "end of packet"
	return NRF_RADIO->EVENTS_END;  // == 1;
}

bool Radio::isDisabled() {
	// i.e. not busy with (in midst of) xmit or rcv
	return NRF_RADIO->EVENTS_DISABLED; // == 1;
}




bool Radio::isCRCValid() {
	return NRF_RADIO->CRCSTATUS == 1;	// CRCOk;
}


// TODO reduce coupling, transport shouldn't know this
void Radio::clearPacketDoneEvent() {
	NRF_RADIO->EVENTS_END = 0;
}
