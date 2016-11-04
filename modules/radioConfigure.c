
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
}

uint32_t RadioDevice::frequency(){ return NRF_RADIO->FREQUENCY; }


void RadioDevice::configureWhiteningSeed(int value){
	// assert(value < )
	NRF_RADIO->DATAWHITEIV = value & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;
	// Whitening enabled elsewhere
}

void RadioDevice::configureWhiteningOn() {
	// !!! Must not destroy contents of PCNF1, configured previously
	// Configuring packet format later destroys this.
	NRF_RADIO->PCNF1 = NRF_RADIO->PCNF1		// Bit set
			| ((1 << RADIO_PCNF1_WHITEEN_Pos) & RADIO_PCNF1_WHITEEN_Msk);
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
void RadioDevice::configureStaticPayloadFormat(const uint8_t payloadCount, const uint8_t addressLength) {

	// We don't use a mask when bit-oring regs, we assert parameters are not too large
	// assert(payloadCount<256);	// uint8_t guarantees this
	// Nordic docs disallow AddressLength 2 but others have reported it works
	assert(addressLength >= 2);	// see "Disable standard addressing" on DevZone
	assert(addressLength <= 5);
	NRF_RADIO->PCNF1 =
			  (payloadCount << RADIO_PCNF1_MAXLEN_Pos)  // max length of payload
			| (payloadCount << RADIO_PCNF1_STATLEN_Pos) // expand payload (over LENGTH) with 0 bytes
			| ((addressLength-1) << RADIO_PCNF1_BALEN_Pos);	// base address length in number of bytes.

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




void RadioDevice::configurePacketAddress(BufferPointer data){
	/*
	 *  point to packet in memory, pointer must fit in 4 byte register
	 *
	 *  Address pointed to must be in "DATA RAM" i.e. writable memory,
	 *  else per datasheet, "may result" in Hardfault (why not "shall").
	 *
	 *  Address pointed to is volatile: both RADIO and mcu may write it.
	 *
	 *  Implementation is portable until platform (ARM) pointers exceed 32-bit.
	 *  Cast a pointer as ordinary 32-bit int.
	 */
	NRF_RADIO->PACKETPTR = (uint32_t) data;
	assert((uint8_t *) (NRF_RADIO->PACKETPTR) ==  data);
}


void RadioDevice::configureXmitPower(unsigned int dBm) {
	// Convert to platform constant
	int8_t value;
	switch(dBm) {
	case 1:
		value = RADIO_TXPOWER_TXPOWER_Pos4dBm;
		break;
	case 8:
		value = RADIO_TXPOWER_TXPOWER_Neg40dBm;
		break;
	default:
		value = RADIO_TXPOWER_TXPOWER_0dBm;
	}
	NRF_RADIO->TXPOWER = value;
}


// Nordic calls it MODE
// Defaults on reset to 1M
// Here omitting certain values (250k) the device supports.
void RadioDevice::configureMegaBitrate(unsigned int baud) {
	int8_t value;
		switch(baud) {
		case 1:
			value = RADIO_MODE_MODE_Nrf_1Mbit;
			break;
		case 2:
		default:
			value = RADIO_MODE_MODE_Nrf_2Mbit;
		}
		NRF_RADIO->MODE = value;
}


/*
 * Reduces rampup from 140uSec (nrf51) to 40uSec.
 */
void RadioDevice::configureFastRampUp() {
#ifdef NRF52
	NRF_RADIO->MODECNF0 = RADIO_MODECNF0_RU_Fast;
	// Not a bitset: alters other fields of the register.
#endif
}

