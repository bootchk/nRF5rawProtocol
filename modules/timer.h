

/*
 * Thin wrapper around NRF SDK app_timer.
 *
 * Uses 32khz xtal clock.
 * 
 */
class Timer {
private:
	// 32/khz divide by 17 is about 1ms per tick
	static const int TimerPrescaler = 16;
	// Only one timer, but +1 ???  See tutorial.
	static const int TimerQueueSize = 2;

	static const int Timeout = 1000;	// units mSec, i.e. 1 second

	public:
		static void init();
		static void createTimers(void (*func)(void*));
		static void restart();
};