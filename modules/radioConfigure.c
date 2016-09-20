
#include "nrf.h"

#include "radioDevice.h"

/*
 * Device level configuration
 *
 * Some device defaults will work.
 * Except that configurePacketFormat must be called since BALEN defaults to invalid value 0?
 */




void RadioDevice::configureFixedFrequency(){
	// FUTURE: parameter
	// Set Freq fixed to one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Convention: whitening seed derived from channel
	// ?? how does 37 derive from channel 2?
	// or 38, 39
	configureWhiteningSeed(37);
}

void RadioDevice::configureWhiteningSeed(int value){
	NRF_RADIO->DATAWHITEIV = value & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;
	// Whitening enabled elsewhere
}


void RadioDevice::configurePacketFormat() {
	// tell radio device the structure of packet in memory and on air
	// Memory structure is S0 (1 byte), LENGTH (1 byte), S1 (1 byte), PAYLOAD (count bytes given by LENGTH)
	// Total length < 258 bytes

	configureOnAirPacketFormat();
	configurePayloadFormat();
}

void RadioDevice::configureOnAirPacketFormat() {
	// All done in bit-fields of PCNF0 register.

	// Note this affects tx and rx.
	// And it is symmetrical, i.e. xmit does not expect S0 exists in RAM if S

	// Do nothing:
	// Defaults to no S0, no LENGTH, no S1
	// I.e. only xmit the payload, without xmit length
	// And the memory format also has these fields (normally a byte) non-existant.

	// RadioHead: NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) & RADIO_PCNF0_LFLEN_Msk); // Length of LENGTH field in bits
	// We are not xmitting LENGTH field.

	// default preamble length.
	// Datasheet says preamble length always one byte??? Conflicts with register description.
}

void RadioDevice::configurePayloadFormat() {
	// 5 bytes, no padding (no length xmitted)
	// 3 bytes address (2 + 1 prefix)
	// All done in PCNF1 register
	// Here we do it in one shot by OR'ing bit fields
	NRF_RADIO->PCNF1 =
			( ((5 << RADIO_PCNF1_MAXLEN_Pos)  & RADIO_PCNF1_MAXLEN_Msk)  // maximum length of payload
			| ((5 << RADIO_PCNF1_STATLEN_Pos) & RADIO_PCNF1_STATLEN_Msk)	// expand the payload (over LENGTH) with 0 bytes
			| ((2 << RADIO_PCNF1_BALEN_Pos) & RADIO_PCNF1_BALEN_Msk))	// base address length in number of bytes.
			| ((1 << RADIO_PCNF1_WHITEEN_Pos) & RADIO_PCNF1_WHITEEN_Msk); 	// enable whiten
			//len-1


	// Note address must be >= 3 bytes see "Disable standard addressing" on DevZone
	// See also recommendations for longer address and CRC:  4 bytes give random noise match every 70 minutes

	// Configuring the address field too i.e. BALEN
	// Note endianess and whitening is also controlled by this register!!!
	// So this code destroys any setting of those bit-fields.
	// FUTURE use BIC and BIS to separate these concerns
}


void RadioDevice::configureCRC() {
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
	NRF_RADIO->CRCPOLY = 0x11021UL; // CRC poly: x^16+x^12^x^5+1
}
