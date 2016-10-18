#include "timer.h"
#include "osClock.h"   // OSTime

/*
 * Class providing clock and sleep
 *
 * Responsibilities:
 * - sleep the system (wake on timeout or other event.)
 * - know reason for waking.
 * - know OSTime from OSClock
 *
 * Owns and uses Timer, which uses app_timer lib from NRF SDK.
 *
 * Specific other event that wakes: radio receives msg.
 *
 * Sleeping puts mcu to idle, low-power mode.
 * Note much power management is automatic by nrf52.
 * E.G. when sleep, all unused peripherals are powered off automatically.
 */



typedef enum {
	MsgReceived,
	TimerExpired,
	None
} ReasonForWake;





class Sleeper {
private:
	static ReasonForWake reasonForWake;
	static Timer timer;


	/*
	 * Callbacks from IRQHandler, so keep short or schedule a task, queue work, etc.
	 * Here we set flag that main event loop reads.
	 *
	 * Passing address, so names can be C++ mangled
	 */
	static void rcvTimeoutTimerCallback(void * p_context);
public:
	// Public because passed to radio so it can hook IRQ into it
	static void msgReceivedCallback();


	static void init();
	static void sleepUntilEventWithTimeout(OSTime);
	static void cancelTimeout();

	// Not in-lined, used by external libraries
	static ReasonForWake getReasonForWake();
	static void clearReasonForWake();


private:
	static void sleepSystemOn();
};


