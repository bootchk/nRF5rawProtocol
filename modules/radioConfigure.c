
#include "nrf.h"

#include "radio.h"

// Low-level concerning configuration

// These are public to the transport layer


void Radio::configureFixedFrequency(){
	// FUTURE: parameter
	// Set Freq fixed to one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Whitening: helpful side effect.
	// Whitening depends on freq?
	NRF_RADIO->DATAWHITEIV = 37 & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;	// or 38, 39
}

/*
 * Receive same address we transmit. (But we don't receive our own transmissions.)
 */
void Radio::configureFixedAddress(){
	// FUTURE: parameter

	// Physical on-air address is set in PREFIX0 + BASE0 by setNetworkAddress
	// Not require it to be set, the default should work?

	NRF_RADIO->TXADDRESS   = 0x00;	// Transmit to logical address 0 (PREFIX0 + BASE0)
	NRF_RADIO->RXADDRESSES = 0x01; // Receive logical address 0 (PREFIX0 + BASE0)
	// There are many RX addresses.  We only chose one.
	// FUTURE setLogicalAddresses()
}


void Radio::configureCRC() {
	NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
	NRF_RADIO->CRCPOLY = 0x11021UL; // CRC poly: x^16+x^12^x^5+1
}
