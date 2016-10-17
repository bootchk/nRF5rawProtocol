#include <cassert>
#include <inttypes.h>
#include "nrf.h"


/*
 * unique ID burned at factory
 * 6 bytes
 */

// Requires long long to hold it
typedef uint64_t DeviceID ;

// But only use LSB six bytes
#define MAX_DEVICE_ID   0xfFfFfFfFfFfF

// FUTURE use a 48-bit bit field


DeviceID myID() {
	/*
	 * NRF_FICR->DEVICEADDR[] is array of 32-bit words.
	 * NRF_FICR->DEVICEADDR yields type (unit32_t*)
	 * Cast: (uint64_t*) NRF_FICR->DEVICEADDR yields type (unit64_t*)
	 * Dereferencing: *(uint64_t*) NRF_FICR->DEVICEADDR yields type uint64_t
	 *
	 * Nordic doc asserts upper two bytes read all ones.
	 */
	uint64_t result = *((uint64_t*) NRF_FICR->DEVICEADDR);

	// Mask off upper bytes, to match over-the-air length of 6 bytes.
	result = result & MAX_DEVICE_ID;

	assert(result <= MAX_DEVICE_ID);
	return result;
}

