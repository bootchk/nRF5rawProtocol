#include "modules/radio.h"

/*
 * API of SleepSyncAgent
 *
 * SleepSyncAgent implements an event loop.
 * A call to loopOnEvents() never returns.
 *
 * SleepSyncAgent calls onWorkMsgQueued() when a work message is queued.
 * That function runs at the same priority as SleepSyncAgent.
 * It should be short to prevent loss of sync.
 * Work messages should be handled in a lower priority thread (say WorkThread.)
 * If the queue does not unblock readers, onWorkMsgQueued()
 * should signal the WorkThread.
 */

// TODo pass the queue.  For now, there is no queue, just the signal.

class SleepSyncAgent {
public:
	SleepSyncAgent(
			Radio* radio,
			void (*onWorkMsgQueued)()
			);
	static void loopOnEvents();	// never returns
};
