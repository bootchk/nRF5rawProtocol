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

// Events and tasks


/*
 * EndOfTransmit AND MsgReceived will BOTH set the same DISABLED event.
 * Device cannot transmit and receive at the same time.
 *
 * Since we use a shortcut, on packet done (TX or RX), radio state becoming DISABLED signifies end.
 *
 * !!! Note we enable interrupt on RX, but not on TX.
 *
 * The RX IRQHandler must clear this event, or another interrupt immediately occurs.
 *
 * The app can also stopReceive() (end receiving before any msg or after one msg), taking state to DISABLED.
 * !!! So state==DISABLED is not always MsgReceived or EndOfTransmit.
 * Must disable any interrupt on DISABLED before calling stopReceive()
 */
void RadioDevice::clearMsgReceivedEvent() {
	clearDisabledEvent();
}
void RadioDevice::clearEndTransmitEvent() {
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

bool RadioDevice::isDisabledState() {
	/*
	 * i.e. not busy with (in midst of) xmit or rcv
	 *
	 * Not the same as EVENTS_DISABLED:  device can be disabled without the event being set.
	 * E.G. it starts disabled.
	 */
	return NRF_RADIO->STATE == RADIO_STATE_STATE_Disabled;
}

// Is radio in middle of receiving packet?
bool RadioDevice::isReceiveInProgressEvent() {
	/*
	 * In progress is indicated by AddressEvent.
	 * Only returns true at most once per started receive.
	 *
	 * NOT by NRF_RADIO->STATE==RADIO_STATE_STATE_Rx, which just indicates radio is listening for addresses.
	 */
	uint32_t result = NRF_RADIO->EVENTS_ADDRESS;
	clearReceiveInProgressEvent();
	return result;
}

void RadioDevice::clearReceiveInProgressEvent() {
	NRF_RADIO->EVENTS_ADDRESS = 0;
	(void) NRF_RADIO->EVENTS_ADDRESS;	// flush ARM write cache
}


bool RadioDevice::isDisabledEventSet() {
	// Used to spin until clear
	return NRF_RADIO->EVENTS_DISABLED; // == 1
}

#ifdef USE_PACKET_DONE_FOR_EOT
/*
 * Event "END" means "end of packet" (RX or TX)
 *
 * Since we use shortcuts, state has already passed through RXIDLE or TXIDLE to DISABLED
 * But the event is still set???? is trigger for interrupt, and must be cleared.
 */
bool RadioDevice::isPacketDone() {
	return NRF_RADIO->EVENTS_END;  // == 1;
}

void RadioDevice::clearPacketDoneEvent() {
	NRF_RADIO->EVENTS_END = 0;
	(void)NRF_RADIO->EVENTS_END;	// flush ARM write buffer
	assert(!isPacketDone());	// ensure
}

void RadioDevice::enableInterruptForPacketDoneEvent() { NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk; }
void RadioDevice::disableInterruptForPacketDoneEvent() { NRF_RADIO->INTENCLR = RADIO_INTENCLR_END_Msk; }
bool RadioDevice::isEnabledInterruptForPacketDoneEvent() { return NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk; }

#endif


/*
 * Interrupts and shortcuts
 *
 * Is there a race between END interrupt and EVENT_DISABLED when shortcut?
 *
 * !!! These just enable Radio to signal interrupt.  Must also:
 * - Nvic.enableRadioIRQ (NVIC is documented by ARM, not by Nordic)
 * - ensure PRIMASK IRQ bit is clear (IRQ enabled in mcu register)
 */

void RadioDevice::enableInterruptForDisabledEvent() { NRF_RADIO->INTENSET = RADIO_INTENSET_DISABLED_Msk; }
void RadioDevice::disableInterruptForDisabledEvent() { NRF_RADIO->INTENCLR = RADIO_INTENCLR_DISABLED_Msk; }
bool RadioDevice::isEnabledInterruptForDisabledEvent() { return NRF_RADIO->INTENSET & RADIO_INTENSET_DISABLED_Msk; }


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
	// !!! Flush Cortex M4 write cache now so radio begins powering up
	(void) NRF_RADIO->POWER;
	assert( value == true ? NRF_RADIO->POWER == 1 : NRF_RADIO->POWER == 0);
}

bool RadioDevice::isPowerOn() { return NRF_RADIO->POWER; }






// CRC

bool RadioDevice::isCRCValid() {
	return NRF_RADIO->CRCSTATUS == 1;	// CRCOk;
}

uint8_t RadioDevice::receivedLogicalAddress() {
	return NRF_RADIO->RXMATCH;
}

