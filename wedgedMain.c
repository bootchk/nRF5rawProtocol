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
	// FUTURE schedule low priority work thread/task to do work
	// realtime constrained
}


Radio radio;
SleepSyncAgent sleepSyncAgent;


int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.init(&radio, onWorkMsgQueued);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
