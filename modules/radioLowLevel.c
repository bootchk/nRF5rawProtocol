
#include "nrf.h"

#include "radioDevice.h"

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

void RadioDevice::enableDCDCPower(){
	// Not really the radio, another peripheral.
	// Per Radiohead
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 0x00000001;
}

void RadioDevice::passPacketAddress(void * data){
	// point to packet in memory, pointer must fit in 4 byte register
	// Is portable until address space exceeds 32-bit
	NRF_RADIO->PACKETPTR = (uint32_t) data;
}


// Events and tasks


/*
 * End Of Transceive (transmission or reception) will set this event.
 *
 * Since we use a shortcut, on packet done, radio state becoming "disabled" signifies EOT.
 * Note we enable interrupt on RX Packet Done (END), but not on TX.
 * The RX ISR must clear  Packet Done (END) event, or another interrupt immediately occurs.
 *
 * The app can also disable() RX, taking state to DISABLED.
 * So state==DISABLED is not always EOT.
 */
void RadioDevice::clearEOTEvent() {
	clearDisabledEvent();
}

void RadioDevice::startRXTask() {
	NRF_RADIO->TASKS_RXEN = 1;	// write TASK register
}
void RadioDevice::startTXTask() {
	NRF_RADIO->TASKS_TXEN = 1;	// write TASK register
}

void RadioDevice::startDisablingTask(){
	NRF_RADIO->TASKS_DISABLE = 1;
}

void RadioDevice::clearDisabledEvent(){
	NRF_RADIO->EVENTS_DISABLED = 0;
	(void)NRF_RADIO->EVENTS_DISABLED;	// flush ARM write buffer
}


void RadioDevice::enableInterruptForPacketDoneEvent() {
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
 * - from state TX   directly to state DISABLED (without explicit DISABLE task, bypassing states TXIDLE and TXDISABLE)
 */
void RadioDevice::setShortcutsAvoidSomeEvents() {
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

void RadioDevice::setRadioPowered(bool value){
	NRF_RADIO->POWER = value;	// 1 or 0 per C++ standard and datasheet
}


// public

// states

bool RadioDevice::isPacketDone() {
	// packet was received.
	// radio state usually RXIDLE or TXIDLE and ???
	// Event "END" means "end of packet"
	return NRF_RADIO->EVENTS_END;  // == 1;
}

bool RadioDevice::isDisabled() {
	// i.e. not busy with (in midst of) xmit or rcv
	return NRF_RADIO->EVENTS_DISABLED; // == 1;
}




bool RadioDevice::isCRCValid() {
	return NRF_RADIO->CRCSTATUS == 1;	// CRCOk;
}


void RadioDevice::clearPacketDoneEvent() {
	NRF_RADIO->EVENTS_END = 0;
	(void)NRF_RADIO->EVENTS_END;	// flush ARM write buffer
}
