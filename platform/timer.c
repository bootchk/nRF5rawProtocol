
#include <cassert>

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "timer.h"
#include "timerService.h"	// TimerPrescaler

/*
 * One-shot timer, restartable.
 *
 * Thin wrapper on app_timer library.
 * See Nordic "Application timer tutorial" for implementation details.
 * The underlying implementation uses several critical sections where interrupts are disabled.
 *
 * The timeoutFunc (callback when timer expires) should be kept short.
 * It is called back in SWI priority??
 *
 * Requires TimerService.init() before Timer.create()
 * Timer does not deeply know the TimerService, but the underlying implemention of Timer
 * knows the underlying implementation of TimerService.
 */



void Timer::create(void (*timerTimeoutHandler)(void*))
{
	// Pass handle to timerData i.e. pointer to pointer
	uint32_t err = app_timer_create(&timerDataPtr,
	            APP_TIMER_MODE_SINGLE_SHOT,
				timerTimeoutHandler);
	APP_ERROR_CHECK(err);
}


void Timer::restartInUnitsMSec(int timeout) {
	// FUTURE migrate conversion to TimerService
	// APP_TIMER_TICKS converts first arg in msec to timer ticks
	uint32_t timeoutTicks = APP_TIMER_TICKS(timeout, TimerService::TimerPrescaler);

	uint32_t err = app_timer_start(timerDataPtr, timeoutTicks, nullptr);
	APP_ERROR_CHECK(err);
}

void Timer::restartInUnitsTicks(uint32_t timeout) {
	// !!! Per Nordic docs, min timeout is 5 ticks.  Else returns NRF_ERROR_INVALID_PARAM
	assert(timeout <= TimerService::MaxTimeout);
	uint32_t err = app_timer_start(timerDataPtr, timeout, nullptr);
	APP_ERROR_CHECK(err);
}

void Timer::cancel(){
	uint32_t err = app_timer_stop(timerDataPtr);
	APP_ERROR_CHECK(err);
}


