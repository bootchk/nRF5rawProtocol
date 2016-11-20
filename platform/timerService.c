
#include <cassert>

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "timerService.h"



namespace {

/*
 * Needed by RTC1 which is needed by app_timer.
 * Not needed if SoftDevice is used.
 */
void initLowFreqXtalOsc() {
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


}  // namespace


void TimerService::init(){
	initLowFreqXtalOsc();

	// Null scheduler function
	//APP_TIMER_INIT(TimerPrescaler, TimerQueueSize, nullptr);
	static uint32_t appTimerBuffer[CEIL_DIV(APP_TIMER_BUF_SIZE(TimerQueueSize),  sizeof(uint32_t))];
	uint32_t err = app_timer_init(TimerPrescaler,
			TimerQueueSize + 1,
			appTimerBuffer,
			NULL);	// nullptr);
	APP_ERROR_CHECK(err);
	// not assert OSClock is running, until app_timer needs it
}


static bool TimerService::isOSClockRunning(){

}
