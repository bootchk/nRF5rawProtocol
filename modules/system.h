
#pragma once

#include "nrf.h"


/*
 * Miscellaneous other peripheral devices
 */


class Nvic {
public:
	static void enableRadioIRQ();
	static void disableRadioIRQ();
};

class PowerSupply {
public:
	static void enableDCDCPower();
};
