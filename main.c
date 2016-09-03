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

#include "modules/transport.h"

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;
void toggleLEDs() {
	for (int i = 0; i < LEDS_NUMBER; i++)
	{
		LEDS_INVERT(1 << leds_list[i]);
		// nrf_delay_ms(500);
	}
}


int main(void)
{
	RawTransport transport;

	transport.powerOn();
	transport.configure();

    // Configure LED-pins as outputs.
    LEDS_CONFIGURE(LEDS_MASK);

    while (true)
    {
    	int buf;

    	// Basic test loop:  xmit, listen, toggleLeds when hear message
    	transport.transmit(&buf);
    	transport.startReceiver();
    	// wait for msg or timeout(timeout);
    	if (true) toggleLEDs();	// isMessageReceived
    	transport.stopReceiver();
    	// If delay is small, led will toggle almost continuously except when there are collisions
    	// delay
    }
}

