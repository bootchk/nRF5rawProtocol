#include <cassert>
#include <inttypes.h>
#include <string.h>	// memcpy

#include "nrf.h"

#include "radio.h"

// Low-level concerning configuration of address


/*
 * Fixed: receive same, single address we transmit. (But we don't receive our own transmissions.)
 */
void Radio::configureFixedLogicalAddress(){
	// FUTURE: parameter

	NRF_RADIO->TXADDRESS   = 0x00;	// Transmit to logical address 0 (defined by PREFIX0.A + BASE0)

	// There are eight RX addresses.  We only enable one (setting one bit out of eight).
	NRF_RADIO->RXADDRESSES = 0x01; // Enable receive logical address 0 (PREFIX0.A + BASE0)
}


/*
 * A pool (more or less a table) of 8 physical (network) addresses, indexed by logical address [0..7].
 *
 * These addresses are not necessarily related to Bluetooth MAC addresses.
 *
 * The 8 prefixes are packed into PREFIX0..PREFIX1
 * The 2 bases are in BASE0..BASE1
 */
void Radio::configureNetworkAddressPool() {
	// Power on default (prefix 0, base 0) is adequate, but not optimal
	// Optimal address extends preamble

	// We only configure first logical address

	uint8_t default_network_address[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	setNetworkAddress(default_network_address, sizeof(default_network_address));


}

void Radio::setNetworkAddress(uint8_t* address, uint8_t len)
{
	// !!! See "RADIO.Address configuration" in data sheet
	// !!! During xmit, base address register is truncated starting from LSByte (when BALEN < 4)
	// !!! LSByte of truncated address is first transmitted, and should not be one of four outlawed values.
	// !!! Prefix is last byte transmitted ("prefix" is misnomer.)

	assert(len >= 3 && len <= 5);
	// FUTURE assert LSB of base is not outlawed value: x00, xFF, x55, xAA

	// This implementation is flawed, but adequate for my purposes.

	// First byte is the prefix
	NRF_RADIO->PREFIX0	  = ((address[0] << RADIO_PREFIX0_AP0_Pos) & RADIO_PREFIX0_AP0_Msk);

	// remainder are base
	uint32_t base;
	memcpy(&base, address+1, len-1);
	NRF_RADIO->BASE0 = base;
	// This leaves default 0 in MSBytes???

	// See also receive example, radio_config.h

	// Alternately, from ??? example
	//NRF_RADIO->PREFIX1	= ((m_alt_aa >> 24) & 0x000000FF);
	//NRF_RADIO->BASE1    = ((m_alt_aa <<  8) & 0xFFFFFF00);
	// See below, same result, for BALEN==3

	// Alternately, from ble_mesh:
	/* Configure Access Address according to the BLE standard */
	// NRF_RADIO->PREFIX0 = 0x8e;
	// NRF_RADIO->BASE0 = 0x89bed600;
	// Here, BALEN==3, so first byte transmitted is d6, not outlawed
}

