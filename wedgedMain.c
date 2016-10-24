/*
 * Main is library that implements an application
 *
 * SleepSync library
 */


#include "modules/radio.h"
#include "platform/sleeper.h"
#include "platform/ledLogger.h"
#include "sleepSyncAgent.h"


LEDLogger ledLogger;

void onWorkMsg(int work) {
	// SleepSyncAgent received and queued a work msg.
	// FUTURE schedule low priority work thread/task to do work
	// realtime constrained
	(void) work;
}

void onSyncPoint() {
	ledLogger.toggleLED(1);
}


Radio myRadio;
Mailbox myMailbox;
SleepSyncAgent sleepSyncAgent;


int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	// TODO start timer to periodically mail work

	sleepSyncAgent.init(&myRadio, &myMailbox, onWorkMsg, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
