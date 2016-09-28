#include <inttypes.h>
#include "nrf.h"



// Miscellaneous functions made available to library wedge


// unique ID burned at factory

uint64_t myID() {
	// DEVICEADDR[] is array of 32-bit words
	// DEVICEADDR yields address of LS word of two 32-bit words
	// returning DEVICEADDR dereferences it into a uint64_t
	return NRF_FICR->DEVICEADDR;
}



/*
 * 24-bits of free-running counter, in LSB of 32-bit word
 *
 * Since RTC is started/stopped by app_timer, must keep a Timer scheduled
 * to ensure app_timer does not stop RTC.
 *
 * Roll over handled by library wedge.
 *
 * app_timer uses RTC1
 * SoftDevice uses RTC0
 * nrf52 has additional RTC2
 *
 * To save power, use RTC1 shared with app_timer
 */
 uint32_t OSClockTicks() {
	 return NRF_RTC1->COUNTER;
 }
