
#include "nrf.h"

#include "system.h"


void PowerSupply::enableDCDCPower(){
	// Enabling DCDC converter lets the radio control it automatically.  Enabling does not turn it on.
	// DCDC converter requires Vcc >= 2.1V and should be disabled below that.
	NRF_POWER->DCDCEN = 1;
	// Takes effect on next system bus read (flushes ARM M4 write buffer)
}




void Nvic::enableRadioIRQ() {
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_EnableIRQ(RADIO_IRQn);
}

void Nvic::disableRadioIRQ() {
	NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_DisableIRQ(RADIO_IRQn);
}

#ifdef FUTURE
Its not clear how to do this with CMSIS functions
bool isEnabledRadioIRQ() {

}
#endif
