
#pragma once

/*
 * Nasty hack: these includes are for the implementation, not the API.
 * Better to hide these.
 * For now, any caller must define include paths:
 * - to the nordic SDK/components/libraries/timer directory where app_timer.h is located,
 * - to the nordic SDK/components/libraries/util directory where app_error.h is located,
 * - to the nordic SDK/components/device directory where nrf.h is located,
 * - to the nordic SDK/components/toolchain/cmsis/include directory where core_cm4.h is located,
 * - to the nordic SDK/components/toolchain/ directory where system_nrf52.h is located,
 * - to the nordic SDK/components/drivers_nrf/nrf_soc_nosd/ directory where nrf_error.h is located,
 * - to nRFrawProtocol to find sdk_config.h
 */
#include <sdk_config.h>
#include <app_timer.h>

#include "osClock.h"  // OSTime

/*
 * A classic OneShot Timer
 *
 * Thin wrapper around NRF SDK app_timer.
 *
 * Uses TimerService.
 * Delegate implementation to app_timer library.
 * That library defines certain struct types.
 * 
 * Note this class is not singleton static.
 */
class Timer {
private:
	// Data members

	/*
	 * Taken from APP_TIMER_DEF macro
	 */
	app_timer_t          timerData = { {0} };
	const app_timer_id_t timerDataPtr = &timerData;

public:
	void create(void (*timeoutFunc)(void*));
	void restartInUnitsMSec(int timeout);
	// Units OSTicks i.e. resolution of RTC1 counter
	void restartInUnitsTicks(OSTime timeout);
	void cancel();
};
