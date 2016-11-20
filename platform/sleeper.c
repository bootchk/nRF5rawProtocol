
#include <cassert>

#include "sleeper.h"
#include "nrf.h"	// SEV, WEV


namespace {

//TimerService Sleeper::timerService;


} // namespace


// static data member
ReasonForWake Sleeper::reasonForWake;
Timer Sleeper::timer;
TimerService Sleeper::timerService;



void Sleeper::init() {
	timerService.init();
	timer.createTimers(rcvTimeoutTimerCallback);
	timer.startPlaceholder();	// runs forever, does nothing but keep OSClock running
	assert(timer.isOSClockRunning());
}


void Sleeper::sleepUntilEventWithTimeout(OSTime timeout) {
	// units are ticks, when RTC has zero prescaler: 30uSec

	// TODO should we be clearing, or asserting (rely on caller to clear, because of races?)
	clearReasonForWake();
	if (timeout < 5) {
		/*
		 * Less than minimum required by restartInTicks() of app_timer library.
		 * Don't sleep, but set reason for waking.
		 * I.E. simulate a sleep.
		 */
		reasonForWake = TimerExpired;
	}
	else {
		timer.restartInTicks(timeout);	// oneshot timer must not trigger before we sleep, else sleep forever
		sleepSystemOn();	// wake by received msg or timeout
		// assert IRQ
	}
	// We either never slept and simulated reasonForWake == Timeout,
	// or slept then woke and the handler (at SWI0 priority of APP_LOW) set reasonForWake in [Timeout, MsgReceived)
	// or an unexpected event woke us.
	// Because of the latter possiblity we can't assert(reasonForWake != Cleared);
}


void Sleeper::cancelTimeout(){
	timer.cancelTimeout();
}



/*
 * IRQ handler callbacks
 *
 * Since there are two concurrent devices, there is a race to set reasonForWake
 *
 * !!! Note using a placeholderTimer, which occasionally wakes us and does not set reasonForWake.
 */

void Sleeper::rcvTimeoutTimerCallback(void * p_context) {
	(void) p_context;
	if (reasonForWake == None)	// if msg didn't arrive just ahead of timeout, before main could cancel timeout
		reasonForWake = TimerExpired;
}
void Sleeper::msgReceivedCallback() {
	/*
	 * If msg arrives after main read reasonForWake and before it stopped the receiver,
	 * this reason will go ignored and msg lost.
	 *
	 * If msg arrives immediately after a timeout but before main has read reasonForWake,
	 * the msg will be handled instead of the timeout.
	 */
	reasonForWake = MsgReceived;
}

ReasonForWake Sleeper::getReasonForWake() { return reasonForWake; }
void Sleeper::clearReasonForWake() { reasonForWake = None; }


/*
 * nrf52:
 * - will not wake from "system off" by a timer (only reset or GPIO pin change.)
 * - in "system on", current is ~3uA == Ion + Irtc + Ix32k
 * No worry about RAM retention in "system on"
 * Here "system" means mcu.
 * Internal event flag is NOT same e.g. RADIO.EVENT_DONE.
 * Internal event flag is set by RTI in ISR.
 *
 * !!! Note using a placeholderTimer, which occasionally wakes us and does not set reasonForWake.
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
