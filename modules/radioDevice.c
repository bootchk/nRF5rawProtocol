#include <cassert>

#include "nrf.h"

#include "radioDevice.h"

/*
 * More implementation of RadioDevice is in radioConfigure.c and radioAddress.c
 *
 * These are here so that most knowledge of a kind of register manipulation is in one place.
 * I don't use comments at a higher level (Radio) to explain register manipulation, the name suffices.
 *
 */

/*
 * Clear means: clear the flag that indicates an event.
 * You start a task, which runs in the radio device.
 * The task eventually sets an event.
 * The event can trigger an interrupt.
 *
 * Per Nordic document "Migrating nrf51 to nrf52", you must read event register after clearing to flush ARM write buffer.
 */


// FUTURE inline

void RadioDevice::enableDCDCPower(){
	// Not really the radio, another peripheral.
	// Per Radiohead
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 0x00000001;
}

void RadioDevice::passPacketAddress(BufferPointer data){
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
 * The RX IRQHandler must clear  Packet Done (END) event, or another interrupt immediately occurs.
 *
 * The app can also disable() RX, taking state to DISABLED.
 * So state==DISABLED is not always EOT.
 */
void RadioDevice::clearEOTEvent() {
	clearDisabledEvent();
}

void RadioDevice::startRXTask() {
	NRF_RADIO->TASKS_RXEN = 1;
}
void RadioDevice::startTXTask() {
	NRF_RADIO->TASKS_TXEN = 1;
}

void RadioDevice::startDisablingTask(){
	NRF_RADIO->TASKS_DISABLE = 1;
}

void RadioDevice::clearDisabledEvent(){
	NRF_RADIO->EVENTS_DISABLED = 0;
	(void)NRF_RADIO->EVENTS_DISABLED;	// flush ARM write buffer
}

bool RadioDevice::isDisabled() {
	/*
	 * i.e. not busy with (in midst of) xmit or rcv
	 *
	 * Not the same as EVENTS_DISABLED:  device can be disabled without the event being set.
	 * E.G. it starts disabled.
	 */
	return NRF_RADIO->STATE == RADIO_STATE_STATE_Disabled;
}

bool RadioDevice::isDisabledEventSet() {
	// Used to spin until clear
	return NRF_RADIO->EVENTS_DISABLED; // == 1
}


/*
 * Event "END" means "end of packet" (RX or TX)
 *
 * Since we use shortcuts, state has already passed through RXIDLE or TXIDLE to DISABLED
 * But the event is still set, is trigger for interrupt, and must be cleared.
 */
bool RadioDevice::isPacketDone() {
	return NRF_RADIO->EVENTS_END;  // == 1;
}

void RadioDevice::clearPacketDoneEvent() {
	NRF_RADIO->EVENTS_END = 0;
	(void)NRF_RADIO->EVENTS_END;	// flush ARM write buffer
	assert(!isPacketDone());	// ensure
}



/*
 * Interrupts and shortcuts
 *
 * Is there a race between END interrupt and EVENT_DISABLED when shortcut?
 * Maybe we should use interrupt on DISABLED instead of END
 */

void RadioDevice::enableInterruptForPacketDoneEvent() { NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk; }
void RadioDevice::disableInterruptForPacketDoneEvent() { NRF_RADIO->INTENCLR = RADIO_INTENCLR_END_Msk; }
bool RadioDevice::isEnabledInterruptForPacketDoneEvent() { return NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk; }

// TODO we never disable the interrupt

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




// Power

void RadioDevice::setRadioPowered(bool value){
	NRF_RADIO->POWER = value;	// 1 or 0 per C++ standard and datasheet
}








// CRC

bool RadioDevice::isCRCValid() {
	return NRF_RADIO->CRCSTATUS == 1;	// CRCOk;
}



