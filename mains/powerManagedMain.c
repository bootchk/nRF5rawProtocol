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
//#include "platform/ledFlasher.h"

#include "platform/logger.h"
#include "platform/powerManager.h"
#include "platform/timerService.h"

#include "worker.h"

#include "sleepSyncAgent.h"

#include "nrf_delay.h"	// For debugging

// LEDLogger ledLogger;
//LEDFlasher ledFlasher;
Worker worker;

Radio myRadio;
Mailbox myOutMailbox;
SleepSyncAgent sleepSyncAgent;
PowerManager powerManager;
TimerService timerService;



void sendWork() {
	if (myOutMailbox.isMail() ){
		// My last mail didn't go out yet
		log("Mail still in mailbox\n");
	}
	else {
		myOutMailbox.put(333);
		log("App put work\n");
	}
}

void randomlySendWork() {
	if (rand() % 5 == 1) {
		sendWork();
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
		// do work at the managed amount
		worker.work();
	}
	// else omit work, not enough power
}


// TODO this is not right yet
// Should do work, out of sync, if voltage > 2.5
void manageVoltageByWork() {
	/*
	 * Part of the total voltage/power management.
	 * This is high side.
	 * See also SyncSleep, which also assists power managment on the low side.
	 *
	 * Here, if voltage is climbing, do more work.
	 * Note work is asynchronous, more work does not require more time in this routine.
	 */
	if (powerManager.isGreaterThanMaxVoltageForWork()) {
		// e.g. > 2.5V
		if (powerManager.isExcessVoltage()) {
			// e.g. > 2.7V
			worker.increaseAmount();
		}
		else {
			// not change amount of work
		}

		/*
		 * This is at syncpoint (but it may be drifted.)
		 * Do work even without receiving work, to manage my own power.
		 * Other units need not work at the same time.
		 */
		worker.work();
		sendWork();
	}
	else {
		// e.g. < 2.5V
		if (powerManager.isLessThanMaxVoltageForWork()) {
			// e.g. 2.1-2.3V
			worker.decreaseAmount();
			/*
			 * Is
			 */not work, until others tell me to
		}
		else {
			/*
			 * Leave amount same, voltage in range [minVForWork, maxVForWork]
			 */
			// e.g. 2.5-2.7V
		}

		// enough power, use it for work
		// This may be out of sync.
		worker.work();
	}
	else {
		// Next work done (if ever) is least amount.
		worker.setLeastAmount();
	}
}


/*
 * Must be short duration, else interferes with sync.
 */
void onSyncPoint() {
	// managing voltage requires periodic checking, this is a convenient place
	manageVoltageByWork();

	// If I have enough power to work, send work to others, they may ignore it
	if (powerManager.isPowerForWork()) {
		randomlySendWork();
	}
	// debug indication, when power supply is not constrained
	// ledLogger.toggleLED(1);
}




int powerManagedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	/*
	 * sleepSyncAgent owns and inits Sleeper instance which requires TimerService.
	 */
	timerService.init();
	worker.init();


	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
