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
	static bool isOSClockRunning();

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

	// Not in-lined, used external libraries
	static ReasonForWake getReasonForWake();
	static void clearReasonForWake();


private:
	static void sleepSystemOn();
};


