#include <inttypes.h>
#include "nrf.h"



// Miscellaneous functions made available to library wedge


/*
 * unique ID burned at factory
 * 6 bytes
 */

uint64_t myID() {
	/*
	 * NRF_FICR->DEVICEADDR[] is array of 32-bit words.
	 * NRF_FICR->DEVICEADDR yields type (unit32_t*)
	 * Cast: (uint64_t*) NRF_FICR->DEVICEADDR yields type (unit64_t*)
	 * Dereferencing: *(uint64_t*) NRF_FICR->DEVICEADDR yields type uint64_t
	 *
	 * Upper two bytes should be all ones.
	 */
	uint64_t result = *((uint64_t*) NRF_FICR->DEVICEADDR);
	// TODO assert all ones upper two bytes
	return result;
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
