/*
 * A main() for testing isolated from real application.
 */

// c++ includes
#include <cassert>
#include <inttypes.h>

#include <cstdlib>	// rand

#ifdef FUTURE
// lkk Can't get this to work
#define NRF_LOG_ENABLED 1
#define NRF_LOG_DEFAULT_LEVEL 0
#define NRF_LOG_USES_UART = 1
#endif
#include "SEGGER_RTT.h"

#ifdef FUTURE
// Macro definitions.
// Leave NRF_LOG macros in place, these define them null ifndef NRF_LOG_ENABLED
#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log
#endif

#include "modules/radio.h"

#include "platform/timer.h"
#include "platform/ledLogger.h"
#include "platform/sleeper.h"



Radio radio;


// Debugging code optional for production

LEDLogger ledLogger;

/*
 /extern "C" {
ret_code_t nrf_log_init(nrf_log_timestamp_func_t timestamp_func);
}
*/

#ifdef FUTURE
void initLogging(void)
{
    // Initialize logging library.
	__attribute__((unused)) uint32_t err_code = NRF_LOG_INIT((nrf_log_timestamp_func_t) NULL);
    // APP_ERROR_CHECK(err_code);
}
#endif

Sleeper sleeper;


void sleepWithRadioOff(){
	// FUTURE
	// assert LFCLK (RTC w xtal) is on
	// current ~10uA
	// sleep with RAM retention: state describes clique
	// random delay here
	int foo = rand();
	foo = foo & 0x3;	// lower two bits
	sleeper.sleepUntilEventWithTimeout(10000 * foo);
}

void fillBufferWithConstant() {
	uint8_t* buffer = radio.getBufferAddress();
	for(int i=0; i<Radio::FixedPayloadCount; i++)
	    	buffer[i]=i;
}

bool isBufferFilledWithConstant() {
	uint8_t* buffer = radio.getBufferAddress();
	bool result = true;
	for(int i=0; i<Radio::FixedPayloadCount; i++) {
		if (buffer[i] != i) {
			result = false;
			break;
		}
	}
	return result;
}



/*
 * Test Raw wireless stack.
 * Basic loop:  xmit, rcv, sleep.
 * When testing with two units:
 * - LED1 should toggle every loop to indicate still functioning
 * - Either LED2 and (LED3 OR LED4) should toggle every loop
 *      LED3 received valid packet from other unit
 *      LED4 received invalid packet (CRC wrong) from other
 *      LED2 not received from other unit
 * Note it is toggle, not blink.  E.g. LED1 is off one loop, on the next loop.
 *
 * It should run indefinitely.
 * If there is an exception, LED1 will stop toggling.
 *
 * Interrupt handlers defined in gcc_startup_nrf52.s
 *
 * main event loop basic state machine: [xmittedThenListening, woken for message, woken for timeout]
 */
void testMain(void)
{
	//initLogging();	// debug
	SEGGER_RTT_Init();
	SEGGER_RTT_WriteString(0, "Hello\r\n");

	sleeper.init();

	radio.init(&sleeper.msgReceivedCallback);
	radio.powerOff();	// for case not reset i.e. using debugger

    // Debug configure LED-pins as outputs, default to on?

    ledLogger.init();
    ledLogger.toggleLEDs();	// off


    // Basic test loop:  xmit, listen, toggleLeds when hear message
    while (true)
    {
    	//NRF_LOG_INFO("Here\n");
    	SEGGER_RTT_WriteString(0, "Hello\r\n");

    	// On custom board (BLE Nano) with only one LED, this is only indication app is working.
    	ledLogger.toggleLED(1);	//

    	// assert configuration is lost after power is cycled
    	radio.powerOnAndConfigure();	// Configures for fixed length messages

    	assert(radio.isDisabledState());	// powerOn (initial entry) and stopReceiver (loop) ensures this


    	// TODO DYNAMIC radio.transmit(rxAndTxBuffer, 5);

    	fillBufferWithConstant();

    	radio.transmitStaticSynchronously();
    	// assert xmit is complete (radio is synchronous to mcu)

    	assert(radio.isDisabledState());	// radio disabled when xmit complete but still powered on

    	sleeper.clearReasonForWake();
    	// DYNAMIC radio.receive(rxAndTxBuffer, 5);
    	radio.receiveStatic();
    	// Receiving with interrupt enabled.
    	assert(radio.isEnabledInterruptForMsgReceived());

    	// Sleep until msg received or timeout
    	// units of .03 uSec ticks
    	sleeper.sleepUntilEventWithTimeout(30000);

    	// If using nrf52DK with many LED's show result
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( sleeper.getReasonForWake() ) {
    	case MsgReceived:
    		// !!! Note toggling of LED 2 usually done in radio.c on every receive
    		if (radio.isPacketCRCValid()) {
    			// stop if data is garbled despite CRC valid
    			assert(isBufferFilledWithConstant());
    		}
    		else {
    			ledLogger.toggleLED(3);
    		}
    		break;

    	case TimerExpired:
    		// !!! LED's scarce, currently using this for invalid packets.
    		//ledLogger.toggleLED(4);
    		break;
    	case None:
    	default:
    		assert(false); // Unexpected

    	}

    	// assert receiver still enabled
    	radio.stopReceive();
    	assert(! radio.isEnabledInterruptForMsgReceived());
    	// assert radioIRQ is disabled
    	radio.powerOff();

    	sleepWithRadioOff();
    	// FUTURE sleep sync, analyze whether two units get in lockstep, missing each other's xmits
    }
}

