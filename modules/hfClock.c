#include <cassert>

#include "nrf.h"	// import NRF_CLOCK

#include "hfClock.h"

/*
 * Read carefully Nordic docs.
 *
 * The hf clock has two sources:
 * - internal RC oscillator (or PLL?):  HFINT
 * - external crystal:   HXFO
 * The HXFO is more accurate (smaller ppm error.)
 * The HXFO uses more power??? so typically the HFINT is used.
 *
 * The hf clock is automatically started when the mcu is on and not sleeping,
 * because the hf clock provides the clock for the mcu.
 * The hf clock enters a power saving mode when the mcu (or other peripherals) don't need it.
 *
 * You control the source by enabling task HFCLKSTARTED and HFCLKSTOP.
 * !!! These specifically start the HXFO !!!
 * You can read the source in HFCLKSTAT (but it is not writeable.)
 *
 * The radio requires HXFO (to precisely delimit bits on the carrier?)
 */
void HfClock::startXtalSource(){
	// Enable the High Frequency clock to the system as a whole
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	// No need to flush ARM write cache, the read below will do it
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	/* Wait for the external oscillator to start up */
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }

	// FUTURE use nrf_driver/hal functions
	assert(NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk);	// 1 == Xtal
	assert(NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKRUN_STATUS_Msk);	// 1 == running
}


void HfClock::stopXtalSource(){
	NRF_CLOCK->TASKS_HFCLKSTOP = 1;
	// not spinning for event indicating stopped
}
