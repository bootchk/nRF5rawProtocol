/*
 * Main is library that implements an application
 *
 * SleepSync library
 */


#include "modules/radio.h"
#include "platform/sleeper.h"
#include "sleepSyncAgent.h"


void onWorkMsgQueued() {
	// SleepSyncAgent received and queued a work msg.
	// TODO schedule low priority work thread/task to do work
	// realtime constrained
}


Radio radio;
SleepSyncAgent sleepSyncAgent(&radio, onWorkMsgQueued);
Sleeper sleeper;


int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	// sleepSyncAgent calls Sleeper methods
	sleeper.init();


	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
