#include "modules/radio.h"
#include "platform/mailbox.h"

// !!! A copy, see original at sleepSyncAgent project

class SleepSyncAgent {

public:
	static void init(
			Radio*,
			Mailbox*,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);
	static void loopOnEvents();	// never returns
};
