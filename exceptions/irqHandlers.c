
/*
 * Handlers for unused IRQ for debugging purposes
 *
 * A definition overrides the default definition in startup_nrf52.s.
 * If you don't define it here, all default definitions are one routing,
 * and reported as the same "WDT" unhandled interrupt
 * (because the last name defined for the same address is WDT.)
 *
 * Note that we are using app_timer which uses POWER_CLOCK and SWI0 interrupt handlers.
 */


// avoid c++ name mangling
// Doesn't work #pragma interrupt??
// Do not define as static (meaning local) because then it does not override the default handlers!!!
extern "C" {


void WDT_IRQHandler() {
	while (1);
}

}
