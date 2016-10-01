/*
 * A main() for testing isolated from real application.
 */

// c++ includes
#include <cassert>
#include <inttypes.h>

// lkk Can't get this to work
//#define NRF_LOG_ENABLED 1
//#define NRF_LOG_DEFAULT_LEVEL 0

#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log

#include "modules/radio.h"

#include "platform/timer.h"
#include "platform/ledLogger.h"
#include "platform/sleeper.h"



// Volatile: radio and mcu both write
//TODO DYNAMIC  volatile uint8_t rxAndTxBuffer[Radio::PayloadCount];	// Must be as large as configured payload length.



// Debugging code optional for production

LEDLogger ledLogger;

/*
 /extern "C" {
ret_code_t nrf_log_init(nrf_log_timestamp_func_t timestamp_func);
}
*/

void initLogging(void)
{
    // Initialize logging library.
	__attribute__((unused)) uint32_t err_code = NRF_LOG_INIT((nrf_log_timestamp_func_t) NULL);
    // APP_ERROR_CHECK(err_code);
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
void testMain(void)
{
	Radio radio;
	Sleeper sleeper;

	initLogging();	// debug

	sleeper.init();

	radio.init(sleeper.msgReceivedCallback);

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
    	radio.powerOnAndConfigure();	// Configures for fixed length messages

    	assert(radio.isDisabled());	// powerOn (initial entry) and stopReceiver (loop) ensures this


    	// TODO DYNAMIC radio.transmit(rxAndTxBuffer, 5);

    	// TODO put something in buffer
    	radio.transmitStaticSynchronously();
    	// assert xmit is complete (radio is synchronous to mcu)

    	assert(radio.isDisabled());	// radio disabled when xmit complete but still powered on

    	sleeper.clearReasonForWake();
    	// DYNAMIC radio.receive(rxAndTxBuffer, 5);
    	radio.receiveStatic();
    	assert(radio.isEnabledInterruptForEOT());

    	sleeper.sleepUntilEventWithTimeout(1000);

    	// If using nrf52DK with many LED's show result
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( sleeper.getReasonForWake() ) {
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

    	// assert receiver still enabled
    	radio.stopReceive();
    	assert(! radio.isEnabledInterruptForEOT());
    	// assert radioIRQ is disabled
    	radio.powerOff();

    	sleepWithRadioOff();
    	// FUTURE sleep sync, analyze whether two units get in lockstep, missing each other's xmits
    }
}

