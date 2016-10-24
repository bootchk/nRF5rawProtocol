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

void onSyncPoint() {

}

Radio myRadio;
Mailbox myMailbox;
SleepSyncAgent sleepSyncAgent;


int wedgedMain() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.init(&myRadio, &myMailbox, onWorkMsgQueued, onSyncPoint);
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
