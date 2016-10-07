
#include <cassert>

#include "sleeper.h"
#include "nrf.h"


// static data member
ReasonForWake Sleeper::reasonForWake;
Timer Sleeper::timer;


void Sleeper::init() {
	timer.init();
	timer.createTimers(rcvTimeoutTimerCallback);
	timer.startPlaceholder();	// runs forever, does nothing but keep OSClock running
	assert(timer.isOSClockRunning());
}

bool Sleeper::isOSClockRunning() { return timer.isOSClockRunning(); }


void Sleeper::sleepUntilEventWithTimeout(OSTime timeout) {
	// units are ticks, when RTC has zero prescaler: 30uSec

	// TODO should we be clearing, or asserting (rely on caller to clear, because of races?)
	reasonForWake = Cleared;
	if (timeout < 5) {
		/*
		 * Less than minimum required by restartInTicks().
		 * Don't sleep, but set reason for waking.
		 * I.E. simulate a sleep.
		 */
		reasonForWake = Timeout;
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

// !!! Note rcvTimeoutTimerCallback is implemented in-line in sleeper.h
void Sleeper::msgReceivedCallback() { reasonForWake = MsgReceived; }

ReasonForWake Sleeper::getReasonForWake() { return reasonForWake; }
void Sleeper::clearReasonForWake() { reasonForWake = Cleared; }


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
