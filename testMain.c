/*
 * A main() for testing isolated from real application.
 */

// c++ includes
#include <cassert>
#include <inttypes.h>

#include <cstdlib>	// rand






#include "modules/radio.h"

#include "platform/timer.h"
#include "platform/ledLogger.h"
#include "platform/sleeper.h"

#include "platform/logger.h"



Radio radio;


// Debugging code optional for production
LEDLogger ledLogger;

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
	initLogging();	// debug

	log("Hello\r\n");

	sleeper.init();

	radio.init(&sleeper.msgReceivedCallback);
	radio.powerOff();	// for case not reset i.e. using debugger

    // Debug configure LED-pins as outputs, default to on?

    ledLogger.init();
    ledLogger.toggleLEDs();	// off


    // Basic test loop:  xmit, listen, toggleLeds when hear message
    while (true)
    {
    	log("Hello");

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
    		//assert(false); // Unexpected
    		;
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

