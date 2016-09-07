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


//#include <stdbool.h>
//#include <stdint.h>
// #include "nrf_delay.h"
#include <inttypes.h>
//#include "nrf_gpio.h"
#include "boards.h"

// For app_timer
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "modules/transport.h"

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;
void toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		LEDS_INVERT(1 << leds_list[i]);
		// nrf_delay_ms(500);
	}
}

bool isMessageReceived;	// flag set by callback

void msgReceivedCallback() {
	// just indicate state in app's basic state machine: [xmittedThenListening, woken for message, woken for timeout]
	isMessageReceived = true;
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


// Stuff needed for app_timer

// 32/khz divide by 17 is about 1ms per tick
const int TimerPrescaler = 16;
// Only one timer, but +1 ???  See tutorial.
const int TimerQueueSize = 2;

void initTimerBasedOnLowFreqXtalClock() {
	// Null scheduler function
	//int unused =
			APP_TIMER_INIT(TimerPrescaler, TimerQueueSize, NULL);	// C11 nullptr);
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {

}





/*
 * This is main without SD (SoftDevice i.e. Nordic-provided wireless stack)
 * Interrupt handlers defined in gcc_startup_nrf52.s
 * Waits are low-level
 */
int main(void)
{
	initTimerBasedOnLowFreqXtalClock();

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
    	// assert radio disabled when xmit complete but still powered on

    	transport.startReceiver();

    	// TODO timer for timeout
    	// wait for msg or timeout(timeout);
    	sleep();

    	if ( isMessageReceived ) {
    		toggleLEDs();
    		isMessageReceived = false;
    	}
    	else {
    		// timed out
    	}

    	transport.stopReceiver();
    	// If timeout is small, led will toggle almost continuously except when there are collisions?
    	// TODO analyze whether two units get in lockstep, missing each other's xmits

    	// TODO larger delay here


    	// TODO test with radio power off in each cycle
    }
}

