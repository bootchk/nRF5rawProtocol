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
			myOutMailbox.put(333);
			log("App put work\n");
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

	// led 3 now means: work received (and thus in sync.)
	// Also used for SyncAgent role change?
	ledLogger.toggleLED(3);
}

void onSyncPoint() {
	// testing
	randomlySendWork();
	ledLogger.toggleLED(1);
}




int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
