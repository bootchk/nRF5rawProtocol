
/*
 * Driver for radio peripheral
 *
 * Understands registers, etc.
 * Does not understand any wireless protocol.
 * Provides services for wireless protocols.
 *
 * Singleton, all static class methods.
 */
class Radio {

public:
	// use in ISR vector table, not call directly
	static void eventHandler();

	static void configure();
	static void transmit(void * data);

	static void startReceiver();
	static void stopReceiver();

	static void powerOn();
	static void powerOff();
	static bool isReady();

private:
	static bool isDisabled();
	static bool wasTransmitting();

	static bool isPacketDone();
	static void clearPacketDoneFlag();

	static bool isCRCValid();
	static void dispatchPacketCallback();

	static void setupXmitOrRcv(void * data);
	static void startXmit();
	static void startRcv();
};
