#pragma once

#include <inttypes.h>
#include "hfClock.h"
#include "radioDevice.h"
#include "system.h"  // Nvic
#include "../platform/types.h"


typedef enum {
	Receiving,
	Transmitting,
	Idle,
	PowerOff
} RadioState;


/*
 * High level driver for radio peripheral
 *
 * Understands collaboration of low-level devices:
 * - HfClock and RadioDevice
 * - DCDCPower and RadioDevice
 *
 * Not understand registers, i.e no dependence on nrf.h
 *
 * Understands wireless protocol:
 * - configures device for protocol
 * - abstracts protocol state from device state
 * - abstracts away the task/event architecture of device
 * - implements interrupt RX vs. spinning TX
 * - understands half-duplex (RX exclusive of TX)
 *
 * Protocol is defined by:
 * - constants for lengths, channels, bitrate
 * - behaviour
 * -- no acks xmitted as in ESB
 * -- all units use one address
 *
 * Singleton, all static class methods.
 */
/*
 * Algebra of valid call sequences:
 *
 *  Typical:
 *    init(), powerOnAndConfigure(), getBufferAddress(), <put payload in buffer> transmitStaticSynchronously(),
 *            receiveStatic(), sleepUntilEventWithTimeout(),  if isDisabledState() getBufferAddress(); <get payload from buffer>,
 *            powerOff(), powerOnAndConfigure(), ...
 *
 *  init() must be called once after mcu POR:
 *    mcu.reset(), init(), powerOnAndConfigure(), ...., mcu.reset(), init(), ...
 *
 *  configure() must be called after powerOn() (use convenience function powerOnAndConfigure())
 *    init(), powerOn(), configureStatic(), receiveStatic, ...
 *
 *  transmitStaticSynchronously blocks, but receiveStatic does not.  If reasonForWake is not MsgReceived,
 *  you must stopReceive() before next radio operation:
 *    receiveStatic(), sleepUntilEventWithTimeout(),
 *      if sleeper.reasonForWake() != MsgReceived then stopReceive()
 *
 *  When reasonForWake is MsgReceived, buffer is full and radio is ready for next transmit or receive.
 *  A packet received can have an invalidCRC.
 *  A packet received having validCRC can be garbled (when count of bit errors greater than CRC can detect.)
 *     receivedStatic(), sleepUntilEventWithTimeout(), if sleeper.reasonForWake() == MsgReceived then
 *        getBuffer(), if isPacketCRCValid() then <use received buffer>,
 *        transmitStaticSynchronously()
 *
 *  You can transmit or receive in any order, but Radio is half-duplex:
 *    init(), powerOnAndConfigure(), receiveStatic(), ...stopReceive(),
 *       receiveStatic(), ..., stopReceive
 *       transmitStaticSynchronously(), ...
 *
 *	Radio isDisabledState() is true after certain other operations:
 *	   powerOnAndConfigure(), assert(isDisabledState())
 *	   transmitStaticSynchronously(), assert(isDisabledState())
 *	   stopReceive(), assert(isDisabledState())
 *	   receiveStatic(), sleepUntilEventWithTimeout(), if sleeper.reasonForWake() == MsgReceived assert(isDisabledState())
 *
 *	Radio isDisabledState() is false at least for a short time after a receive()
 *	but if a packet is received, is true
 *	    receiveStatic(), assert(! isDisabledState()), ..<packet received>, assert(isDisabledState())
 *
 *  Radio is has a single buffer, non-queuing.  After consecutive operations, first contents of buffer are overwritten.
 */
class Radio {
public:
	// Define protocol lengths

	// FUTURE Variable payload up to 258 bytes?

	/*
	 * Fixed: all payloads same size.
	 * Device configured to not transmit S0, LENGTH, S1
	 * Buffer not include S0, LENGTH, S1 fields.
	 */
	static const uint8_t FixedPayloadCount = 10;

	static const uint8_t LongNetworkAddressLength = 4;	// 1 byte preamble, 3 bytes base
	static const uint8_t MediumNetworkAddressLength = 3;	// 1 byte preamble, 2 bytes base
	static const uint8_t ShortNetworkAddressLength = 2;	// 1 byte preamble, 1 bytes base

	//static volatile int guard[10];
	static volatile uint8_t radioBuffer[FixedPayloadCount+60];
	//static volatile int guard2[10];

	static RadioState state;


private:
	// peripherals
	static HfClock hfClock;
	static RadioDevice device;
	static Nvic nvic;
	static PowerSupply powerSupply;

	// OBSOLETE static bool wasTransmitting;  // false: wasReceiving
	static void (*aRcvMsgCallback)();


public:
	static void receivedEventHandler();

	static void init(void (*onRcvMsgCallback)());
	static void configureStatic();
	// platform independent 1: +4, 8: -40, else 0.   Units dBm.
	// FUTURE enum
	static void configureXmitPower(unsigned int dBm);

	static void powerOnAndConfigure();
	static void powerOff();
	static bool isPowerOn();

	static bool isDisabledState();
	static bool isEnabledInterruptForPacketDoneEvent();

	// FUTURE DYNAMIC static void getBufferAddressAndLength(uint8_t** handle, uint8_t* lengthPtr);
	// Can't define in-line, is exported
	static BufferPointer getBufferAddress();


	// Static: buffer owned by radio, of fixed length
	static void transmitStatic();
	static void transmitStaticSynchronously();
	static void receiveStatic();

	static bool isPacketCRCValid();

	static bool isEnabledInterruptForMsgReceived();
	static bool isEnabledInterruptForEndTransmit();

#ifdef DYNAMIC
	static void transmit(BufferPointer data, uint8_t length);
	static void transmitSynchronously(BufferPointer data, uint8_t length);
	static void receive(BufferPointer data, uint8_t length);
#endif

	static void stopXmit();
	static void stopReceive();

	static void spinUntilXmitComplete();

private:
	static void setupFixedDMA();

	static void powerOn();	// public uses powerOnAndConfigure, a radio on is useless without configuration
	static void spinUntilReady();
	static void dispatchPacketCallback();

	static void disable();
	static void spinUntilDisabled();
#ifdef DYNAMIC
	static void setupXmitOrRcv(BufferPointer data, uint8_t length);
#endif
	static void setupInterruptForMsgReceivedEvent();

	static void startXmit();
	static void startRcv();

	static void enableRXTask();
	static void enableTXTask();

	static bool isEventForMsgReceivedInterrupt();
	static void clearEventForMsgReceivedInterrupt();
	static void enableInterruptForMsgReceived();
	static void disableInterruptForMsgReceived();
	static void disableInterruptForEndTransmit();
};
