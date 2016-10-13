#include <inttypes.h>
#include "nrf.h"


/*
 * unique ID burned at factory
 * 6 bytes
 */

// TODO typedef UnitID or MasterID

uint64_t myID() {
	/*
	 * NRF_FICR->DEVICEADDR[] is array of 32-bit words.
	 * NRF_FICR->DEVICEADDR yields type (unit32_t*)
	 * Cast: (uint64_t*) NRF_FICR->DEVICEADDR yields type (unit64_t*)
	 * Dereferencing: *(uint64_t*) NRF_FICR->DEVICEADDR yields type uint64_t
	 *
	 * Nordic doc asserts upper two bytes all ones.
	 */
	uint64_t result = *((uint64_t*) NRF_FICR->DEVICEADDR);
	// FUTURE assert all ones upper two bytes
	return result;
}

