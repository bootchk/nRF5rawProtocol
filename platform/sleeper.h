#include "timer.h"

/*
 * Sleep system, wake oh timeout or other event.
 * Knows reason for waking.
 *
 * Specific other event: radio receives msg.
 *
 * Sleeping puts mcu to idle, low-power mode.
 * Note much power management is automatic by nrf52.
 * E.G. when sleep, all unused peripherals are powered off automatically.
 *
 * Uses Timer, which uses app_timer lib from NRF SDK.
 */



typedef enum {
	MsgReceived,
	Timeout,
	Cleared
} ReasonForWake;


typedef uint32_t OSTime;


class Sleeper {
private:
	static ReasonForWake reasonForWake;
	static Timer timer;

public:
	static void init();

	/*
	 * Callbacks from IRQHandler, so keep short or schedule a task, queue work, etc.
	 * Here we set flag that main event loop reads.
	 *
	 * Passing address, so names can be C++ mangled
	 */
private:
	static void rcvTimeoutTimerCallback(void * p_context) { reasonForWake = Timeout; }
public:
	// Public because passed to radio so it can hook IRQ into it
	static void msgReceivedCallback();



	static void sleepUntilEventWithTimeout(OSTime);

	// Public, but in-lined, not used by external libraries, SleepSyncAgent?

	static ReasonForWake getReasonForWake() { return reasonForWake; }

	// Not in-lined
	static void clearReasonForWake();
	static bool reasonForWakeIsMsgReceived();
	static bool reasonForWakeIsTimerExpired();
	static bool reasonForWakeIsCleared();

private:
	static void sleepSystemOn();
};


