
//#define USE_TESTMAIN 1





#ifdef USE_TESTMAIN

// Test without real app
void testMain();

int main() {
	testMain();
}

#else

// Test with a library that implements an application
// Here: SleepSync library

#include "modules/radio.h"
#include "sleepSyncAgent.h"


void onWorkMsgQueued() {
	// SleepSyncAgent received and queued a work msg.
	// TODO schedule low priority work thread/task to do work
	// realtime constrained
}


Radio radio;
SleepSyncAgent sleepSyncAgent(&radio, onWorkMsgQueued);


int main() {
	// assert embedded system startup is done and calls main.
	// assert platform initialized radio

	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}

#endif
