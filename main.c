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

// lkk Can't get this to work
//#define NRF_LOG_ENABLED 1
//#define NRF_LOG_DEFAULT_LEVEL 0

#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log


#include "modules/timer.h"
#include "modules/transport.h"


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





bool isMessageReceived;	// flag set by callback


// Callbacks from interrupts.
// extern C so names not mangled
// TODO but I'm passing address, so names can be mangled
extern "C" {


void rcvTimeoutTimerHandler(void * p_context)
{
	// set global flag indicating reason for waking
	isMessageReceived = false;
	toggleLEDThree();
}

void msgReceivedCallback() {
	// just indicate state in app's basic state machine: [xmittedThenListening, woken for message, woken for timeout]
	isMessageReceived = true;
}

}


void sleep() {
	// Enter System ON sleep mode

	// Make sure any pending events are cleared
	__SEV();
	__WFE();
	// This should actually sleep until the next event.
	__WFE();

	// For more information on the WFE - SEV - WFE sequence, please refer to the following Devzone article:
	// https://devzone.nordicsemi.com/index.php/how-do-you-put-the-nrf51822-chip-to-sleep#reply-1589

	// There is conflicting advice about the proper order.  This order seems to work.
}



/*
 * This is main without SD (SoftDevice i.e. Nordic-provided wireless stack)
 * Interrupt handlers defined in gcc_startup_nrf52.s
 */
int main(void)
{
	Timer timer;

	initLogging();	// debug

	timer.init();
	timer.createTimers(rcvTimeoutTimerHandler);

	RawTransport transport;

	transport.init(msgReceivedCallback);

	// TODO pass callback to transport

    // Debug configure LED-pins as outputs, default to on?
    LEDS_CONFIGURE(LEDS_MASK);
    toggleLEDs();	// off

    while (true)
    {
    	int buf;

    	NRF_LOG_INFO("Here\n");

    	// Basic test loop:  xmit, listen, toggleLeds when hear message

    	// assert configuration is lost after power is cycled
    	transport.powerOn();
    	transport.configure();

    	assert(transport.isDisabled());	// powerOn (initial entry) and stopReceiver (loop) ensures this

    	transport.transmit(&buf);
    	// assert xmit is NOT complete (radio is asynchronous to mcu)
    	transport.spinUntilXmitComplete();
    	// assert xmit is complete

    	assert(transport.isDisabled());	// radio disabled when xmit complete but still powered on

    	assert(! isMessageReceived);	// We cleared the flag earlier.
    	transport.startReceiver();

    	timer.restart();	// timer must not trigger before we sleep
    	sleep();	// wait for received msg or timeout

    	// Some interrupt woke us up and set a flag
    	if ( isMessageReceived ) {
    		toggleLEDOne();
    		isMessageReceived = false;
    	}
    	else {
    		// timed out
    		toggleLEDTwo();
    		// assert receiver still enabled
    	}

    	transport.stopReceiver();
    	transport.powerOff();

    	// TODO analyze whether two units get in lockstep, missing each other's xmits
    	// TODO random delay here
    	// timer.restart();
    	// sleep();
    }
}

