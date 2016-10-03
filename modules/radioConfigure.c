
#include <cassert>
#include "nrf.h"

#include "radioDevice.h"

/*
 * Device level configuration
 *
 * Some device defaults will work.
 * Except that configurePacketFormat must be called since BALEN defaults to invalid value 0?
 */



// TODO rename channel
void RadioDevice::configureFixedFrequency(){
	// FUTURE: parameter
	// Set Freq fixed to one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Convention: whitening seed derived from channel
	// ?? how does 37 derive from channel 2?
	// or 38, 39
	configureWhiteningSeed(37);
}
uint32_t RadioDevice::frequency(){ return NRF_RADIO->FREQUENCY; }


void RadioDevice::configureWhiteningSeed(int value){
	NRF_RADIO->DATAWHITEIV = value & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;
	// Whitening enabled elsewhere
}


/*
 * From Nordic docs:
 * Memory structure is S0 (1 byte), LENGTH (1 byte), S1 (1 byte), PAYLOAD (count bytes given by LENGTH)
 * On-air format may be smaller: less bits for S0, LENGTH, S1
 * Total length < 258 bytes
 */
/*
 * Static:
 * LENGTH not transmitted (sender and receiver agree on fixed length)
 * S0, S1 not transmitted
 *
 * Only needs to be configured once.
 */
void RadioDevice::configureStaticPacketFormat(const uint8_t PayloadCount, const uint8_t AddressLength) {
	configureStaticOnAirPacketFormat();
	configureStaticPayloadFormat(PayloadCount, AddressLength);
}

void RadioDevice::configureStaticOnAirPacketFormat() {
	// All done in bit-fields of PCNF0 register.

	// Note this affects tx and rx.
	// And it is symmetrical, i.e. xmit does not expect S0 exists in RAM if S

	// Do nothing:
	// Defaults to no S0, no LENGTH, no S1
	// I.e. only xmit the payload, without xmit length
	// And the memory format also has these fields (normally a byte) non-existant.

	// RadioHead: NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) ); // Length of LENGTH field in bits
	// We are not xmitting LENGTH field.

	// default preamble length.
	// Datasheet says preamble length always one byte??? Conflicts with register description.
}

/*
 * Since not xmitting LENGTH, both MAXLEN and STALEN = PayloadCount bytes.
 * xmit exactly PayloadCount
 * rcv PayloadCount (and truncate excess)
 */
void RadioDevice::configureStaticPayloadFormat(const uint8_t PayloadCount, const uint8_t AddressLength) {

	// TODO we don't need the mask if we are certain parameters are not too large
	assert(PayloadCount<256);
	assert(AddressLength >= 3);	// see "Disable standard addressing" on DevZone
	assert(AddressLength <= 5);
	NRF_RADIO->PCNF1 =
			  (PayloadCount << RADIO_PCNF1_MAXLEN_Pos)  // max length of payload
			| (PayloadCount << RADIO_PCNF1_STATLEN_Pos) // expand payload (over LENGTH) with 0 bytes
			| ((AddressLength-1) << RADIO_PCNF1_BALEN_Pos)	// base address length in number of bytes.
			| (1 << RADIO_PCNF1_WHITEEN_Pos); 	       // enable whiten

	/*
	 * See also Nordic recommendations to use long address
	 * and to include address in CRC:
	 * 4 byte address gives random noise match every 70 minutes,
	 */

	/*
	 * Implementation: one shot by OR'ing bit fields
	 * This is also configuring:
	 * - address field i.e. BALEN
	 * - endianess and whitening
	 * So this code destroys any previous setting of those bit-fields.
	 * FUTURE use BIC and BIS to separate these concerns
	 */
}


/*
 * Knows the choice of CRC.
 *
 * A good design choice for CRC depends on the length of data covered by CRC.
 *
 * Whether CRC includes address is also configurable.
 * Fixed payload of 10 bytes plus 3 bytes address yields 13 bytes or 104 bits
 */
void RadioDevice::configureCRC() {
	// CRC appropriate to data length about 120 bits: shorter, and better polynomial

	// This defines:
	// - which MSB bit of generator is fed back (bit 8, 16, or 24)
	// - how many bytes of calculated CRC are transmitted.
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos);  // count bytes of CRC

	// The generator is at most 3 bytes long.
	// CRCCNF_LEN defines which MSB bit is fed back, so the initial value can be 3 bytes
	// even if the CRC length is
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value of CRC generator

	// Usually called CCITT-16, defined as 0x8810 but here 0x11021 == 1 << 0x8810
	// There are better polynomials (e.g. Baicheva 0xC86C) for 16-bit CRC
	// NRF_RADIO->CRCPOLY = 0x11021UL; // CRC poly: x^16+x^12^x^5+1

	configureCRCPolynomialForShortData();
}

void RadioDevice::configureCRCPolynomialForShortData() {
	// Usually called C2, defined as 0x97 853210, but here 0x12F == 1 << 0x97
	// A good CRC for 8-bit CRC and data length about 120 bits
	NRF_RADIO->CRCPOLY = 0x12FUL;
}


void RadioDevice::configurePacketAddress(BufferPointer data){
	/*
	 *  point to packet in memory, pointer must fit in 4 byte register
	 *
	 *  Address pointed to must be in "DATA RAM" i.e. writable memory,
	 *  else per datasheet, "may result" in Hardfault (why not "shall").
	 *
	 *  Address pointed to is volatile: both RADIO and mcu may write it.
	 *
	 *  Implementation is portable until address space exceeds 32-bit
	 */
	NRF_RADIO->PACKETPTR = (uint32_t) data;
}
