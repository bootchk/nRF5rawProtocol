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

