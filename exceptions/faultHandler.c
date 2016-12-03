

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

/*
 * Power off any used devices (specific to app) and sleep in a loop.
 * So that the mcu does not brownout, but the app is halted (non-functional.)
 *
 * If instead, you brownout, the mcu resets and might appear to continue functioning,
 * with no indication that a fault occurred.
 */
void sleepForeverInLowPower() {
	//

	__disable_irq();

	// TODO turn off RTC
	/*
	 * App uses app_timer lib
	 * It is configured to leave RTC running.
	 * To turn it off, see app_timer.c, something like NRF_RTC->TASKS_STOP=1;
	 */

	// power off radio
	NRF_RADIO->POWER = 0;
	(void) NRF_RADIO->POWER;	// flush ARM write cache

	while(true) {
		// Sleep in low power
		__SEV();
		__WFE();	// Since internal event flag is set, this clears it without sleeping
		__WFE();
		// if any event, continue to sleep
	}
}


/*
 * Conditionally compile:
 * - Release mode: reset
 * - Debug mode: sleep forever in low power
 */
void resetOrHalt() {
    // On fault, the system can only recover with a reset.
#ifndef DEBUG
    NVIC_SystemReset();
#else
    sleepForeverInLowPower();
#endif // DEBUG
}


/*
 * Handlers for exceptions/faults.
 * These override the defaults, which are infinite loops and quickly brownout a low-power supply.
 */

/*
 * Handler for:
 * - NRF_SDK library errors.
 * - assert() macro (it calls this)
 *
 * The default handler does: app_error_save_and_stop(id, pc, info);
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
	(void) id;	// unused params
	(void) pc;
	(void) info;

	resetOrHalt();
}

/*
 * Handlers for hardware faults.
 *
 * nRF51 subset (nRF52 has more)
 *
 * !!! We don't use an OS:
 * only HardFault might occur.
 *
 * unused means: not used in C++ but overrides an ASM definition
 */

// Certain peripherals or SW. Non-maskable, only preempted by reset
__attribute__ ((unused))
static void NMI_Handler(void)  { resetOrHalt(); }

// M0 bus faults and other hw faults
// M4 further splits this out.
__attribute__ ((unused))
static void HardFault_Handler(void) { resetOrHalt(); }

// call to OS: SVC instruction executed
__attribute__ ((unused))
static void SVC_Handler(void) { resetOrHalt(); }

// OS Context switching
__attribute__ ((unused))
static void PendSV_Handler(void) { resetOrHalt(); }

// OS clock
__attribute__ ((unused))
static void SysTick_Handler(void) { resetOrHalt(); }


} // extern C
