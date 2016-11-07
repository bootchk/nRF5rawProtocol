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

#include "nrf_delay.h"	// For debugging

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


void testLEDFlash() {
	// Test minimum perceivable LED flash
	// Web says 0.15ms should be perceptible.
	// 1ms is barely perceptible in normal ambient light
	// 5ms is adequate
	// 10ms is just as bright as always on

	ledLogger.init();
	ledLogger.toggleLED(1);
	while (true) {
		ledLogger.toggleLED(1);
		nrf_delay_ms(5);
		ledLogger.toggleLED(1);
		nrf_delay_ms(1000);
	}
}

int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
