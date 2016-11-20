
#pragma once

#include <inttypes.h>
#include "osClock.h"  // OSTime

/*
 * Manages set of timers and a clock on which they are based.
 *
 * Thin wrapper on Nordic library "timer" aka app_timer.
 *
 * Implementation notes:
 *
 * !!! app_timer will stop RTCounter unless define APP_TIMER_KEEPS_RTC_ACTIVE=1 in sdk_config.h
 * SleepSyncAgent requires RTCCounter always running, since:
 * LongClock depends on OSClock
 * OSClock depend on RTCounter
 * all of which we want ticking always (except for brownout.)
 */



class TimerService {
private:
	/*
	 * Max timers served.  Plus en extra.  See app_timer tutorial.
	 *
	 * Actual count of timers used by app is defined elsewhere.
	 */
	static const uint8_t TimerQueueSize = 4;

public:
	/*
	 * Prescaler on RTCounter device underlying OSClock.
	 *
	 * Public because it is passed to app_timer macros used by Timer.
	 *
	 * Approximately (ignoring off by 1 details)
	 * when prescaler is X, a tick is (1(32khz/X)) seconds.
	 * X=16 gives about 1mSec per tick.
	 * X=0 gives about 30uSec per tick.
	 */
	static const uint32_t TimerPrescaler = 0;

	/*
	 * Max timeout provided by RTCounter device underlying OSClock.
	 */
	static const OSTime MaxTimeout = 0xFFFFFF;	// 24-bit

public:
	static void init();
	static bool isOSClockRunning();
};
