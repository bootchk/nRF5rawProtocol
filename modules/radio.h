
/*
 * Driver for radio peripheral (understands registers, etc.)
 *
 * Singleton, all static class methods.
 */
class Radio {

public:
	static void configure();
	static void transmit(void * data);
	static void turnOnReceiver();
	// TODO turnOFF
private:
	static void setupXmitOrRcv(void * data);
	static void startXmit();
	static void startRcv();
};
