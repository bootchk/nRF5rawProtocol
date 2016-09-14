
#include "nrf.h"	// import NRF_CLOCK

#include "hfClock.h"


void HfClock::start(){
	// Enable the High Frequency clock to the system as a whole
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	/* Wait for the external oscillator to start up */
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }
}


void HfClock::stop(){
	// TODO
}
