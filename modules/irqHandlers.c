
// Handlers for unused IRQ for debugging purposes
// A definition overrides the default defintion in startup_nrf52.s

// avoid c++ name mangling
// Doesn't work #pragma interrupt??
// Do not define as static (meaning local) because then it does not override the default handlers!!!
extern "C" {


void WDT_IRQHandler() {
	while (1);
}


/*
void POWER_CLOCK_IRQHandler() {
	while (1);
}
*/

}
