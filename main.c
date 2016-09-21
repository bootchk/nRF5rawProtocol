
// c++ includes
#include <cassert>
#include <inttypes.h>

#include "boards.h"

// lkk Can't get this to work
//#define NRF_LOG_ENABLED 1
//#define NRF_LOG_DEFAULT_LEVEL 0

#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log


#include "modules/timer.h"
#include "modules/radio.h"


typedef enum {
	MsgReceived,
	Timeout,
	Cleared
} ReasonForWake;

ReasonForWake reasonForWake;

// Volatile: radio and mcu both write
volatile uint8_t rxAndTxBuffer[Radio::PayloadCount];	// Must be as large as configured payload length.



// Debugging code optional for production

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;
void toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		LEDS_INVERT(1 << leds_list[i]);
		// nrf_delay_ms(500);
	}
}
void toggleLEDOne() { LEDS_INVERT(1 << leds_list[0]); }
void toggleLEDTwo() { LEDS_INVERT(1 << leds_list[1]); }
void toggleLEDThree() { LEDS_INVERT(1 << leds_list[2]); }

/*
 /extern "C" {
ret_code_t nrf_log_init(nrf_log_timestamp_func_t timestamp_func);
}
*/

static void initLogging(void)
{
    // Initialize logging library.
	__attribute__((unused)) uint32_t err_code = NRF_LOG_INIT((nrf_log_timestamp_func_t) NULL);
    // APP_ERROR_CHECK(err_code);
}





/*
 * Callbacks from IRQHandler, so keep short or schedule a task, queue work, etc.
 * Here we set flag that main event loop reads.
 *
 * Passing address, so names can be C++ mangled
 */
void rcvTimeoutTimerCallback(void * p_context) { reasonForWake = Timeout; }

void msgReceivedCallback() { reasonForWake = MsgReceived; }



/*
 * nrf52:
 * - will not wake from "system off" by a timer (only reset or GPIO pin change.)
 * - in "system on", current is ~3uA == Ion + Irtc + Ix32k
 * No worry about RAM retention in "system on"
 * Here "system" means mcu.
 * Internal event flag is NOT same e.g. RADIO.EVENT_DONE.
 * Internal event flag is set by RTI in ISR.
 */
void sleepSytemOn() {

	// Make sure any pending events are cleared
	__SEV();
	__WFE();	// Since internal event flag is set, this clears it without sleeping
	__WFE();	// This should actually sleep until the next event.

	// For more information on the WFE - SEV - WFE sequence, please refer to the following Devzone article:
	// https://devzone.nordicsemi.com/index.php/how-do-you-put-the-nrf51822-chip-to-sleep#reply-1589

	// There is conflicting advice about the proper order.  This order seems to work.
}


void sleepWithRadioOff(){
	// FUTURE
	// assert LFCLK (RTC w xtal) is on
	// current ~10uA
	// sleep with RAM retention: state describes clique
	// random delay here
	// timer.restart();
	// sleep();
}

/*
 * This is main without SD (SoftDevice i.e. Nordic-provided wireless stack)
 * Interrupt handlers defined in gcc_startup_nrf52.s
 *
 * main event loop basic state machine: [xmittedThenListening, woken for message, woken for timeout]
 */
int main(void)
{
	Timer timer;
	Radio radio;

	initLogging();	// debug

	timer.init();
	timer.createTimers(rcvTimeoutTimerCallback);

	radio.init(msgReceivedCallback);

    // Debug configure LED-pins as outputs, default to on?
    LEDS_CONFIGURE(LEDS_MASK);
    toggleLEDs();	// off

    // Basic test loop:  xmit, listen, toggleLeds when hear message
    while (true)
    {
    	NRF_LOG_INFO("Here\n");

    	// On custom board (BLE Nano) with only one LED, this is only indication app is working.
    	toggleLEDOne();	//

    	// assert configuration is lost after power is cycled
    	radio.powerOn();
    	radio.configure();

    	assert(radio.isDisabled());	// powerOn (initial entry) and stopReceiver (loop) ensures this
    	assert(! radio.isEnabledInterruptForPacketDoneEvent());

    	radio.transmit(rxAndTxBuffer);
    	// assert xmit is NOT complete (radio is asynchronous to mcu)
    	radio.spinUntilXmitComplete();
    	// assert xmit is complete

    	assert(radio.isDisabled());	// radio disabled when xmit complete but still powered on

    	reasonForWake = Cleared;
    	radio.receive(rxAndTxBuffer);
    	assert(radio.isEnabledInterruptForPacketDoneEvent());

    	timer.restart();	// oneshot timer must not trigger before we sleep, else sleep forever
    	sleepSytemOn();	// wake by received msg or timeout

    	// If using nrf52DK with many LED's show result
#ifndef BOARD_CUSTOM
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( reasonForWake ) {
    	case MsgReceived:
    		toggleLEDThree();
    		break;

    	case Timeout:
    		toggleLEDTwo();
    		break;
    	default:
    		;
    		// assert(false); // Unexpected
    		// TODO we are getting here, figure it out because it may be corrupting a receive?
    		// See errata, FPU is waking us???
    	}
#endif

    	// assert receiver still enabled
    	radio.stopReceive();
    	radio.powerOff();

    	sleepWithRadioOff();
    	// FUTURE sleep sync, analyze whether two units get in lockstep, missing each other's xmits
    }
}

