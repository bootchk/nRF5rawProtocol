

#include <inttypes.h>
#include "nrf.h"	// SEV, WEV


/*
 * Redefine the default handler defined in SDK/components/libraries/util/app_error_weak.c
 *
 * This is specialized for this ultra-low power app:
 * - in production, soft reset
 * - in debugging, turn off devices (radio and RTC) and sleep in a loop.
 *
 * (The default handler also resets in production,
 * but in DEBUG it spins the mcu at 6ma and soon brownout resets.
 */

extern "C" {


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
	(void) id;	// unused params
	(void) pc;
	(void) info;

    //NRF_LOG_ERROR("Fatal\r\n");
    //NRF_LOG_FINAL_FLUSH();

    // On assert, the system can only recover with a reset.
#ifndef DEBUG
    NVIC_SystemReset();
#else
    // Specific to app

    __disable_irq();

    // TODO turn off RTC

    // power off radio
    NRF_RADIO->POWER = 0;
    // !!! Flush Cortex M4 write cache now so radio begins powering up
    (void) NRF_RADIO->POWER;

    while(true) {
    	// Sleep in low power
    	__SEV();
    	__WFE();	// Since internal event flag is set, this clears it without sleeping
    	__WFE();
    	// if any event, continue to sleep
    }

    // The default handler does: app_error_save_and_stop(id, pc, info);
#endif // DEBUG
}

} // extern C
