/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */


// c++ includes
#include <cassert>
#include <inttypes.h>

#include "boards.h"


#include "modules/timer.h"
#include "modules/transport.h"



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

bool isMessageReceived;	// flag set by callback


// Callbacks from interrupts.
// extern C so names not mangled
// TODO but I'm passing address, so names can be mangled
extern "C" {


void rcvTimeoutTimerHandler(void * p_context)
{
	// set global flag indicating reason for waking
	isMessageReceived = false;
}

void msgReceivedCallback() {
	// just indicate state in app's basic state machine: [xmittedThenListening, woken for message, woken for timeout]
	isMessageReceived = true;
}

}


void sleep() {
	// Enter System ON sleep mode
	__WFE();
	// Make sure any pending events are cleared
	__SEV();
	__WFE();

	// For more information on the WFE - SEV - WFE sequence, please refer to the following Devzone article:
	// https://devzone.nordicsemi.com/index.php/how-do-you-put-the-nrf51822-chip-to-sleep#reply-1589
}



/*
 * This is main without SD (SoftDevice i.e. Nordic-provided wireless stack)
 * Interrupt handlers defined in gcc_startup_nrf52.s
 */
int main(void)
{
	Timer timer;

	timer.init();
	timer.createTimers(rcvTimeoutTimerHandler);

	RawTransport transport;

	// ??? Can we configure before power on?
	// Assume power means: just the transceiver itself, and not the radio's interface
	transport.powerOn();
	transport.configure();
	// TODO pass callback to transport

    // Configure LED-pins as outputs.
    LEDS_CONFIGURE(LEDS_MASK);

    while (true)
    {
    	int buf;

    	// Basic test loop:  xmit, listen, toggleLeds when hear message

    	// After power on, spin wait for ready
    	while (!transport.isReady()) {}

    	transport.transmit(&buf);
    	// assert xmit complete (synchronous)
    	// assert radio disabled when xmit complete but still powered on

    	assert(! isMessageReceived);	// We cleared the flag earlier.
    	transport.startReceiver();

    	timer.restart();	// timer must not trigger before we sleep
    	// wait for msg or timeout
    	sleep();

    	// Some interrupt woke us up and set a flag
    	if ( isMessageReceived ) {
    		toggleLEDOne();
    		isMessageReceived = false;
    	}
    	else {
    		// timed out
    		toggleLEDTwo();
    	}

    	transport.stopReceiver();
    	// If timeout is small, led will toggle almost continuously except when there are collisions?
    	// TODO analyze whether two units get in lockstep, missing each other's xmits

    	// TODO larger delay here


    	// TODO test with radio power off in each cycle
    }
}

