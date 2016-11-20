
#include <cassert>

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "timer.h"
#include "timerService.h"	// TimerPrescaler

/*
 * Hacky, non-general.
 *
 * Two timers:
 * - one-shot, reused by app, times out a sleep while receiver on.
 * - repeating, just keeps app_timer from shutting down RTC1
 * (because app reads RTC1 as free-running clock.)
 *
 * See Nordic "Application timer tutorial"
 */


// static class data
// Macro defined by NRD SDK to define an app_timer instance
// Thus difficult to cram into class??
APP_TIMER_DEF(oneShotTimer);

APP_TIMER_DEF(placeholderTimer);




// type app_timer_timeout_handler_t {aka void (*)(void*)}
void placeholderTimeoutHandler(void*){
	// Nothing.
}


void app_error_fault_handler(uint32_t id, uint32_t lineNum, uint32_t fileName) {
	// gdb break will stop here
	__disable_irq();
	while(true);
	// FUTURE attempt recovery and log
}



bool Timer::isPlaceholderStarted = false;




void Timer::createTimers(void (*timerTimeoutHandler)(void*))
{
	createOneShot(timerTimeoutHandler);
	createPlaceholderTimer();
}

void Timer::createOneShot(void (*timerTimeoutHandler)(void*))
{
	uint32_t err = app_timer_create(&oneShotTimer,
	            APP_TIMER_MODE_SINGLE_SHOT,
				timerTimeoutHandler);
	APP_ERROR_CHECK(err);
}

void Timer::createPlaceholderTimer()
{
	uint32_t err = app_timer_create(&placeholderTimer,
		            APP_TIMER_MODE_REPEATED,
					placeholderTimeoutHandler);
	APP_ERROR_CHECK(err);
}


void Timer::restartInMSec(int timeout) {
	// APP_TIMER_TICKS converts first arg in msec to timer ticks
	uint32_t timeoutTicks = APP_TIMER_TICKS(timeout, TimerService::TimerPrescaler);
	uint32_t err = app_timer_start(oneShotTimer, timeoutTicks, nullptr);
	APP_ERROR_CHECK(err);
}

void Timer::restartInTicks(uint32_t timeout) {
	// !!! Per Nordic docs, min timeout is 5 ticks.  Else returns NRF_ERROR_INVALID_PARAM
	assert(timeout <= TimerService::MaxTimeout);
	uint32_t err = app_timer_start(oneShotTimer, timeout, nullptr);
	APP_ERROR_CHECK(err);
}

void Timer::cancelTimeout(){
	uint32_t err = app_timer_stop(oneShotTimer);
	APP_ERROR_CHECK(err);
}

void Timer::startPlaceholder() {

	// By starting with max possible timeout, it expires and repeats as infrequently as possible
	// saving cpu cycles.
	uint32_t err = app_timer_start(placeholderTimer, TimerService::MaxTimeout, nullptr);
	APP_ERROR_CHECK(err);

	isPlaceholderStarted = true;
	// assert OSClock is running, since app_timer needs it
}

bool Timer::isOSClockRunning() {
	return isPlaceholderStarted;
}


#ifdef FUTURE
Not exposed by app_timer.h
bool Timer::isPlaceholderRunning() {
	// Copied from app_timer.c
	return (((timer_node_t*)*placeholderTimer)->is_running);
}
#endif

