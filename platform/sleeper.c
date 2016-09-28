

#include "sleeper.h"
#include "nrf.h"


// static data member
ReasonForWake Sleeper::reasonForWake;
Timer Sleeper::timer;


void Sleeper::init() {
	timer.init();
	timer.createTimers(rcvTimeoutTimerCallback);
	// Assert a placeholding repeating Timer keeps app_timer from stopping RTC clock.
}


void Sleeper::sleepUntilEventWithTimeout(OSTime timeout) {
	// timeout units are 30uSec
	timer.restart(timeout);	// oneshot timer must not trigger before we sleep, else sleep forever
	sleepSystemOn();	// wake by received msg or timeout
}

void Sleeper::msgReceivedCallback() { reasonForWake = MsgReceived; }

bool Sleeper::reasonForWakeIsMsgReceived() { return reasonForWake == MsgReceived; }
bool Sleeper::reasonForWakeIsTimerExpired() { return reasonForWake == Timeout; }


/*
 * nrf52:
 * - will not wake from "system off" by a timer (only reset or GPIO pin change.)
 * - in "system on", current is ~3uA == Ion + Irtc + Ix32k
 * No worry about RAM retention in "system on"
 * Here "system" means mcu.
 * Internal event flag is NOT same e.g. RADIO.EVENT_DONE.
 * Internal event flag is set by RTI in ISR.
 */
void Sleeper::sleepSystemOn() {

	// Make sure any pending events are cleared
	__SEV();
	__WFE();	// Since internal event flag is set, this clears it without sleeping
	__WFE();	// This should actually sleep until the next event.

	// For more information on the WFE - SEV - WFE sequence, please refer to the following Devzone article:
	// https://devzone.nordicsemi.com/index.php/how-do-you-put-the-nrf51822-chip-to-sleep#reply-1589

	// There is conflicting advice about the proper order.  This order seems to work.
}
