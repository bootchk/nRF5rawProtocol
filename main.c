
// c++ includes
#include <cassert>
#include <inttypes.h>



// lkk Can't get this to work
//#define NRF_LOG_ENABLED 1
//#define NRF_LOG_DEFAULT_LEVEL 0

#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log


#include "modules/timer.h"
#include "modules/radio.h"

#include "modules/ledLogger.h"


typedef enum {
	MsgReceived,
	Timeout,
	Cleared
} ReasonForWake;

ReasonForWake reasonForWake;

// Volatile: radio and mcu both write
volatile uint8_t rxAndTxBuffer[Radio::PayloadCount];	// Must be as large as configured payload length.



// Debugging code optional for production

LEDLogger ledLogger;

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

    ledLogger.init();
    ledLogger.toggleLEDs();	// off

    // Basic test loop:  xmit, listen, toggleLeds when hear message
    while (true)
    {
    	NRF_LOG_INFO("Here\n");

    	// On custom board (BLE Nano) with only one LED, this is only indication app is working.
    	ledLogger.toggleLED(1);	//

    	// assert configuration is lost after power is cycled
    	radio.powerOn();
    	radio.configure();

    	assert(radio.isDisabled());	// powerOn (initial entry) and stopReceiver (loop) ensures this

    	radio.transmit(rxAndTxBuffer);
    	// assert xmit is NOT complete (radio is asynchronous to mcu)
    	radio.spinUntilXmitComplete();
    	// assert xmit is complete//leds_list =  LEDS_LIST;

    	assert(radio.isDisabled());	// radio disabled when xmit complete but still powered on

    	reasonForWake = Cleared;
    	radio.receive(rxAndTxBuffer);
    	assert(radio.isEnabledInterruptForEOT());


    	timer.restart();	// oneshot timer must not trigger before we sleep, else sleep forever
    	sleepSytemOn();	// wake by received msg or timeout

    	// If using nrf52DK with many LED's show result
#ifndef BOARD_CUSTOM
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( reasonForWake ) {
    	case MsgReceived:
    		ledLogger.toggleLED(3);
    		break;

    	case Timeout:
    		ledLogger.toggleLED(2);
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
    	assert(! radio.isEnabledInterruptForEOT());
    	// assert radioIRQ is disabled
    	radio.powerOff();

    	sleepWithRadioOff();
    	// FUTURE sleep sync, analyze whether two units get in lockstep, missing each other's xmits
    }
}

