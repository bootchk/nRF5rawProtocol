
#include "nrf.h"

#include "osClock.h"

#ifdef FUTURE
bool OSClock::isRunning() {
	// FUTURE isRunning
	return true;
}
#endif


 OSTime OSClock::ticks() {
	 return NRF_RTC1->COUNTER;
}

