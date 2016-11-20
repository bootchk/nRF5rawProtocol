/*
 * Main is application.
 * But it relinquishes (inversion) control to SleepSync library, which calls back.
 *
 * This is a more advanced test harness for SleepSync,
 * where application and SleepSync are also cognizant of power.
 *
 * The differences are:
 * we flash LED to save power instead of leaving it on
 */


#include <cstdlib>	// rand

#include "modules/radio.h"
#include "platform/sleeper.h"
#include "platform/ledLogger.h"
#include "platform/ledFlasher.h"

#include "platform/logger.h"
#include "platform/powerManager.h"
#include "platform/timerService.h"

#include "sleepSyncAgent.h"

#include "nrf_delay.h"	// For debugging

// LEDLogger ledLogger;
LEDFlasher ledFlasher;

Radio myRadio;
Mailbox myOutMailbox;
SleepSyncAgent sleepSyncAgent;
PowerManager powerManager;
TimerService timerService;


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

		// Work is: blink, not toggle
		ledFlasher.flashLED(1);
	}
	// else omit doing work
}

/*
 * Must be short duration, else interferes with sync.
 */
void onSyncPoint() {

	if (powerManager.isPowerForWork()) {
		randomlySendWork();
	}

	// debug indication, when not low power supply
	// ledLogger.toggleLED(1);

	// TODO drain power
	// Can't do it here because this routine should be kept short.
	if (powerManager.isExcessVoltage()) {
		// Flash here to indicate: plenty of power and alive
		ledFlasher.flashLED(1);
	}
}




int powerManagedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	/*
	 * sleepSyncAgent owns and inits Sleeper instance which requires TimerService.
	 */
	timerService.init();
	ledFlasher.init();
	ledFlasher.flashLED(1);


	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
