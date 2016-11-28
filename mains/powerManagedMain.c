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
		myOutMailbox.put(33);
		log("App put work\n");
	}
}

void randomlyWork() {
	if (rand() % 5 == 1) {
		worker.work();
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


/*
 * Part of the total voltage/power management.
 * This is high side.
 * See also SyncSleep, which also does power managment on the low side.
 *
 * Here, if voltage is climbing, do more work.
 * Note work is asynchronous, more work does not require more time in this routine.
 *
 * This is at syncpoint (but it may be drifted.)
 *
 * This also determines the behaviour of the flashing:
 * - units try to flash in sync
 * - units with excess power may flash when other units can't
 * - when all units have moderate power, some unit may randomly trigger all to flash
 *
 */
void manageVoltageByWork() {

	// Exclusive cases
	if (powerManager.isVoltageExcess()) {
		// e.g. > 2.7V
		/*
		 * Self must work to keep voltage from exceeding Vmax
		 */
		// TODO no randomness, collisions?  Need additional randomness in time of transmittal within slot?
		worker.increaseAmount();
		worker.work();
		sendWork();
	};

	if (powerManager.isVoltageHigh()) {
		// e.g. 2.5V - 2.7V
		/*
		 * I could work.
		 */
		worker.decreaseAmount();
		randomlyWork();
	}

	if (powerManager.isVoltageMedium()) {
		// e.g. 2.3-2.5V
		/*
		 *  not change amount of work
		 *
		 *  not work, until others tell me to
		 */
	};

	if (powerManager.isVoltageLow()) {
		// e.g. 2.1-2.3V

		/*
		 * Not enough power to work.
		 * Next work done (if ever) is least amount.
		 */
		worker.setLeastAmount();
	}
}


/*
 * Must be short duration, else interferes with sync.
 */
void onSyncPoint() {
	// managing voltage requires periodic checking, this is a convenient place
	manageVoltageByWork();

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
