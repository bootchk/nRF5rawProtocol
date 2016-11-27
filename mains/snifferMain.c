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
 *
 * Currently specialized:
 * - logMessage understands SleepSync encoded messages
 * - uses RTT logging
 *
 * To use:
 * - start debugger in Eclipse
 * - start JLinkRTTClient which attaches to debugger and displays log of sniffed messages
 *
 * In the log:
 * - "." means a sleeping period where no message was received
 * - "Bad CRC" means message was received but CRC invalid.
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


/*
 * Understands logical layer is SleepSync.
 */
void logMessage() {
	BufferPointer buffer;

	buffer = radio.getBufferAddress();

	if (buffer[0] == 17)
		// Print two LSB's of MasterID
		SEGGER_RTT_printf(0, "Sync %02x%02x\n", buffer[1], buffer[2]);
	else if (buffer[0] == 136)
		SEGGER_RTT_printf(0, "Work %02x%02x\n", buffer[1], buffer[2]);
	else
		SEGGER_RTT_printf(0, "%02x %02x%02x\n", buffer[0], buffer[1], buffer[2]);
	#ifdef OBSOLETE
	// First byte is message type
	logByte(buffer[0]);

	// MasterID is bytes 1-6 of OTA serialized data
	//dereference value at address of second byte in buffer
	/*
	 * This doesn't work, it gives a hardfault (for unaligned access?)
	long long masterID = *((long long*) &buffer[1]);
	masterID &= 0x00ffffff;	// Mask 6 bytes
	logLongLong(masterID);
	*/

	// print two LSB byte of six bytes of MasterID
	logByte(buffer[1]);
	logByte(buffer[2]);
#endif
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

