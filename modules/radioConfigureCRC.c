#include "nrf.h"

#include "radioDevice.h"


/*
 * Knows choice of CRC.
 *
 * A good design choice for CRC depends on the length of data covered by CRC.
 *
 * Whether CRC includes address is also configurable.
 * Fixed payload of 10 bytes plus 3 bytes address yields 13 bytes or 104 bits
 */
void RadioDevice::configureMediumCRC() {
	// CRC appropriate to data length about 120 bits: shorter, and better polynomial

	// This defines:
	// - which MSB bit of generator is fed back (bit 8, 16, or 24)
	// - how many bytes of calculated CRC are transmitted.
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos);  // count bytes of CRC

	// The generator is at most 3 bytes long.
	// CRCCNF_LEN defines which MSB bit is fed back, so the initial value can be 3 bytes
	// even if the CRC length is less???
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value of CRC generator

	// Usually called CCITT-16, defined as 0x8810 but here 0x11021 == 1 << 0x8810
	// There are better polynomials (e.g. Baicheva 0xC86C) for 16-bit CRC
	NRF_RADIO->CRCPOLY = 0x11021UL; // CRC poly: x^16+x^12^x^5+1
}

void RadioDevice::configureShortCRC() {
	configureShortCRCLength();
	configureShortCRCGeneratorInit();
	configureShortCRCPolynomialForShortData();
}



void RadioDevice::configureShortCRCLength() {
	// This defines:
	// - which MSB bit of generator is fed back (bit 8, 16, or 24)????
	// - how many bytes of calculated CRC are transmitted.
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos);  // count bytes of CRC
}

void RadioDevice::configureShortCRCGeneratorInit(){
	// The generator is at most 3 bytes long.
	// CRCCNF_LEN defines which MSB bit is fed back, so the initial value can be 3 bytes
	// even if the CRC length is less???
	// But here we init with one byte of 1's
	NRF_RADIO->CRCINIT = 0xFFUL;      // Initial value of CRC generator
}

void RadioDevice::configureShortCRCPolynomialForShortData() {
	// Usually called C2, defined as 0x97 853210, but here 0x12F == 1 << 0x97
	// A good CRC for 8-bit CRC and data length about 120 bits
	NRF_RADIO->CRCPOLY = 0x12FUL;
}
