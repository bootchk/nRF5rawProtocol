
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
	static void powerOn();
	static void powerOff();

	static bool isReady();

	static bool isDisabled();
	static bool wasTransmitting();

	static bool isPacketDone();
	static void clearPacketDoneFlag();

	static bool isCRCValid();

	static void setupXmitOrRcv(void * data);
	static void startXmit();
	static void startRcv();

	// Configuration
	static void setFixedFrequency();
	static void setFixedAddress();
};
