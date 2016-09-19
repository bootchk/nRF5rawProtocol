
// For app_timer
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "timer.h"

// Stuff needed for app_timer
// See Nordic "Application timer tutorial"
// Timer is single shot, times out a sleep while receiver on.

// static class data
// Macro defined by NRD SDK to define an app_timer instance
// Thus difficult to cram into class??
APP_TIMER_DEF(rcvTimeoutTimer);


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
}



void Timer::createTimers(void (*timerTimeoutHandler)(void*))
{
    uint32_t err = app_timer_create(&rcvTimeoutTimer,
            APP_TIMER_MODE_SINGLE_SHOT,
			timerTimeoutHandler);
    APP_ERROR_CHECK(err);
}

void Timer::restart() {
	// APP_TIMER_TICKS converts first arg in msec to timer ticks
	uint32_t err = app_timer_start(rcvTimeoutTimer, APP_TIMER_TICKS(Timeout, TimerPrescaler), nullptr);
	    APP_ERROR_CHECK(err);
}

