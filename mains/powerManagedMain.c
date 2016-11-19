/*
 * Main is application.
 * But it relinquishes (inversion) control to SleepSync library, which calls back.
 *
 * This is a more advanced test harness for SleepSync,
 * where application and SleepSync are also cognizant of power.
 */


#include <cstdlib>	// rand

#include "modules/radio.h"
#include "platform/sleeper.h"
#include "platform/ledLogger.h"
#include "platform/logger.h"
#include "platform/powerManager.h"

#include "sleepSyncAgent.h"

#include "nrf_delay.h"	// For debugging

LEDLogger ledLogger;

Radio myRadio;
Mailbox myOutMailbox;
SleepSyncAgent sleepSyncAgent;
PowerManager powerManager;


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
 * The app understands power for work.
 * SleepSyncAgent further understands power for radio.
 *
 * SleepSyncAgent will sync-keep, or sync-maintain depending on power.
 *
 * The app sends work OUT only if self has enough power.
 * SleepSyncAgent will convey work IN if it hears it,
 * but app will not act on it if not enough power..
 */


/*
 * SleepSyncAgent received and queued a work msg.
 * This method is realtime constrained.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	(void) work;	// Unused

	if (powerManager.isPowerForWork()) {

		// TODO blink, not toggle
	// led 3 now means: work received (and thus in sync.)
	// Also used for SyncAgent role change?
	ledLogger.toggleLED(3);
	}
	// else omit doing work
}

void onSyncPoint() {

	if (powerManager.isPowerForWork()) {
		randomlySendWork();
	}

	// debug indication
	ledLogger.toggleLED(1);

	// TODO drain power
	// Can't do it here because this routine should be kept short.
	if (powerManager.isExcessVoltage()) {

	}
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

int powerManagedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
