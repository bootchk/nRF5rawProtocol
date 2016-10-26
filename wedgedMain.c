/*
 * Main is library that implements an application
 *
 * SleepSync library
 */


#include <cstdlib>	// rand

#include "modules/radio.h"
#include "platform/sleeper.h"
#include "platform/ledLogger.h"
#include "platform/logger.h"
#include "sleepSyncAgent.h"



LEDLogger ledLogger;

Radio myRadio;
Mailbox myOutMailbox;
SleepSyncAgent sleepSyncAgent;


void randomlySendWork() {
	if (rand() % 5 == 1) {
		if (myOutMailbox.isMail() ){
			// My last mail didn't go out yet
			log("Mail still in mailbox\n");
		}
		else {
			myOutMailbox.put(1);
			log("Put work\n");
		}
	}
}

/*
 * SleepSyncAgent received and queued a work msg.
 * This method is realtime constrained.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	(void) work;	// Unused

	// led 1 now means: work received (and thus in sync.)
	ledLogger.toggleLED(1);
}

void onSyncPoint() {
	// ledLogger.toggleLED(1);

	// testing
	randomlySendWork();
}




int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	// TODO start timer to periodically mail work

	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
