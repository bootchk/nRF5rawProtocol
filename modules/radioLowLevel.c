
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


// FUTURE inline

void Radio::enableDCDCPower(){
	// Not really the radio, another peripheral.
	// Per Radiohead
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 0x00000001;
}

void Radio::passPacketAddress(void * data){
	// point to packet in memory, pointer must fit in 4 byte register
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

/*
 * The radio emits events for many state transitions we are not interested in.
 * And the radio requires certain tasks to make transitions.
 * A shortcut hooks an event to a transition so that we do not need to explicitly start a task.
 * A shortcut makes for much shorter on-air times.
 *
 * !!! The state diagram also has a transition without a condition:  /Disabled from TXDISABLE to DISABLED.
 *
 * These shortcuts are:
 * - from state TXRU directly to state TX (without explicit start READY task, bypassing state TXIDLE)
 * - from state TX   directly to tstat DISABLED (without explicit DISABLE task, bypassing states TXIDLE and TXDISABLE)
 */
void Radio::setShortcutsAvoidSomeEvents() {
	//
	// In other words, make automatic transitions in state diagram.
	NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk // shortcut READY event to START task
			| RADIO_SHORTS_END_DISABLE_Msk;		 // shortcut END event to DISABLE task
			// | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;	 //

	// RadioHead nrf51
	// These shorts will make the radio transition from Ready to Start to Disable automatically
	// for both TX and RX, which makes for much shorter on-air times
	// NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos)
	//	              | (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);

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


// FUTURE reduce coupling, transport shouldn't know this
void Radio::clearPacketDoneEvent() {
	NRF_RADIO->EVENTS_END = 0;
}
