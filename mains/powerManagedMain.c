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


#include "modules/radio.h"
#include "platform/sleeper.h"
#include "platform/ledLogger.h"
#include "platform/timerService.h"

#include "firefly/groupWork.h"
#include "firefly/workSupervisor.h"

#include "sleepSyncAgent.h"



Radio myRadio;

SleepSyncAgent sleepSyncAgent;
TimerService timerService;
Mailbox myOutMailbox;

WorkSupervisor workSupervisor;


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
 * I.E. other units are working, in sync, so self should work if it can.
 *
 * This method is realtime constrained.
 *
 * FUTURE schedule low priority work thread/task to do work.
 */
void onWorkMsg(WorkPayload work) {
	(void) work;	// Unused

	workSupervisor.tryWorkInIsolation();
}



/*
 * Managing voltage requires periodic checking, this is a convenient place.
 *
 * This may initiate group work (working locally and as a group.)
 *
 * Must be short duration, else interferes with sync.
 * FUTURE make this a thread and yield to higher priority sleep sync thread
 */
void onSyncPoint() {
	workSupervisor.manageVoltageByWork();

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
	workSupervisor.init(&myOutMailbox);


	sleepSyncAgent.init(&myRadio, &myOutMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
