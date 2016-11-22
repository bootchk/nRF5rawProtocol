/*
 * A main() that sniffs other unit broadcasts.
 *
 * Sniff: logs valid message types.
 *
 * Assumes steady power source, but does sleep.
 * Sleeps just to wake and toggle an LED to show alive.
 *
 * LED1 alive but no message
 * LED3 message CRC invalid
 */

// c++ includes
#include <cassert>

#include "modules/radio.h"

#include "platform/timer.h"
#include "platform/ledLogger.h"
#include "platform/sleeper.h"

#include "platform/logger.h"	// Not a class, wraps RTT
#include "platform/timerService.h"



Radio radio;
TimerService timerService;
Sleeper sleeper;
LEDLogger ledLogger;


void logMessage() {
	BufferPointer buffer;

	buffer = radio.getBufferAddress();
	// First byte is message type
	// Crude formatting: longLong format, even though it is only a byte
	logLongLong(buffer[0]);
}


void snifferMain(void)
{
	initLogging();

	timerService.init();
	sleeper.init();	// requires initialized TimerService

	radio.init(&sleeper.msgReceivedCallback);
	radio.powerOff();	// for case not reset i.e. using debugger

    ledLogger.init();
    ledLogger.toggleLEDs();	// off

    log("Hello\r\n");

    // Radio always on
    radio.powerOnAndConfigure();	// Configures for fixed length messages

    while (true)
    {
    	assert(radio.isDisabledState());	// powerOn (initial entry) and stopReceiver (loop) ensures this

    	sleeper.clearReasonForWake();
    	radio.receiveStatic();
    	// Receiving with interrupt enabled.
    	assert(radio.isEnabledInterruptForMsgReceived());

    	// Sleep until msg received or timeout
    	// units of .03 uSec ticks, one second
    	sleeper.sleepUntilEventWithTimeout(30000);

    	// If using nrf52DK with many LED's show result
    	// Some interrupt ??? event woke us up and set reasonForWake
    	switch ( sleeper.getReasonForWake() ) {
    	case MsgReceived:
    		// !!! Note toggling of LED 2 usually done in radio.c on every receive
    		if (radio.isPacketCRCValid()) {
    			logMessage();
    		}
    		else {
    			log("Bad CRC.\n");
    			ledLogger.toggleLED(3);
    		}
    		break;

    	case TimerExpired:
    		// Indicate alive but no message.
    		log(".");
    		ledLogger.toggleLED(1);
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    		break;

    	case None:
    	default:
    		log("Unexpected wake reason\n");
    		//assert(false); // Unexpected
    		;
    		// Put radio in state that next iteration expects.
    		radio.stopReceive();
    	}

    	// assert radio still on but not receiving
    	// continue loop to listen again
    }
}

