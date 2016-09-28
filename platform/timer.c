

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "timer.h"

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
APP_TIMER_DEF(rcvTimeoutTimer);

APP_TIMER_DEF(placeholderTimer);


/*
 * Start LFCLK oscillator.
 * Needed by RTC1 which is needed by app_timer.
 * Not needed if SoftDevice is used.
 */
void initLowFreqXtalClock() {
	uint32_t err = nrf_drv_clock_init();
	APP_ERROR_CHECK(err);
	nrf_drv_clock_lfclk_request(NULL);
}


void placeholderTimeoutHandler(){
	// Nothing.
}


void app_error_fault_handler(uint32_t id, uint32_t lineNum, uint32_t fileName) {
	// gdb break will stop here
	__disable_irq();
	while(true);
	// FUTURE attempt recovery and log
}

void Timer::init() {	//TimerBasedOnLowFreqXtalClock() {
	initLowFreqXtalClock();

	// Null scheduler function
	//APP_TIMER_INIT(TimerPrescaler, TimerQueueSize, nullptr);
	static uint32_t appTimerBuffer[CEIL_DIV(APP_TIMER_BUF_SIZE(TimerQueueSize),  sizeof(uint32_t))];
	uint32_t err = app_timer_init(TimerPrescaler,
			TimerQueueSize + 1,
			appTimerBuffer,
			NULL);	// nullptr);
	APP_ERROR_CHECK(err);

	startPlaceholder();	// runs forever, does nothing
}



void Timer::createTimers(void (*timerTimeoutHandler)(void*))
{
	createOneShot(timerTimeoutHandler);
	createPlaceholderTimer();
}

void Timer::createOneShot(void (*timerTimeoutHandler)(void*))
{
	uint32_t err = app_timer_create(&rcvTimeoutTimer,
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


void Timer::restart(int timeout) {
	// APP_TIMER_TICKS converts first arg in msec to timer ticks
	uint32_t err = app_timer_start(rcvTimeoutTimer, APP_TIMER_TICKS(timeout, TimerPrescaler), nullptr);
	APP_ERROR_CHECK(err);
}

void Timer::startPlaceholder() {
	uint32_t err = app_timer_start(placeholderTimer, MaxTimeout, nullptr);
	APP_ERROR_CHECK(err);
}

