
#include "nrf.h"

#include "osClock.h"

#ifdef FUTURE
bool OSClock::isRunning() {
	// TODO isRunning
	return true;
}
#endif


 OSTime OSClock::ticks() {
	 return NRF_RTC1->COUNTER;
}

