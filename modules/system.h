
#pragma once


/*
 * Miscellaneous other peripheral devices
 */


class Nvic {
public:
	static void enableRadioIRQ();
	static void disableRadioIRQ();
	// FUTURE static bool isEnabledRadioIRQ();
};

class PowerSupply {
public:
	static void enableDCDCPower();
};
