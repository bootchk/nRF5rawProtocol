
#include <cassert>

#include "radio.h"

#include "../platform/ledLogger.h"

LEDLogger ledLogger2;

/*
 * Implementation notes:
 *
 * Interrupts are used.  Alternative is to poll (see RadioHead.)
 * Now, interrupt on receive.  FUTURE: interrupt on xmit.
 *
 * Uses "configuration registers" of the RADIO peripheral(section of the datasheet).
 * AND ALSO "task registers" of the "Peripheral Interface"(section of the datasheet)
 * to the RADIO (for tasks, events, and interrupts.)
 * See device/nrf52.h for definitions of register addresses.
 * Each task and event has its own register.
 *
 * Not keeping our own state (radio peripheral device has a state.)
 *
 * !!! Note this file has no knowledge of registers (nrf.h) , see radioLowLevel and radioConfigure.c
 */


// Private class (singleton) data members
HfClock Radio::hfClock;
RadioDevice Radio::device;
Nvic Radio::nvic;
PowerSupply Radio::powerSupply;

void (*Radio::aRcvMsgCallback)();

// State.  Can't tell from radio device whether xmit or rcv task was started, (when using shortcuts.)
bool Radio::wasTransmitting;

uint8_t Radio::staticBuffer[PayloadCount];




extern "C" {
void RADIO_IRQHandler() {
	Radio::receivedEventHandler();	// relay to static C++ method
	// assert RTI will be called but I don't understand how the compiler knows to do that?
	// assert an internal event flag will be set on RTI that must be cleared by SEV???
}
}

void Radio::receivedEventHandler(void)
{
	// assert(device.isOnlyEnabledInterruptForDisabled());
	// We only expect an interrupt on packet received

    if (isEventForMsgReceivedInterrupt())
    {
    	clearEventForMsgReceivedInterrupt();
    	ledLogger2.toggleLED(2);	// LED 2 show every receive

        if (isValidPacket()) {
        	// assert buffer is valid received data, side effect
        	dispatchPacketCallback();
        	ledLogger2.toggleLED(3);	// LED 3 valid received
        }
        else {
        	// ignore invalid packet
        	// assert(false);
        	ledLogger2.toggleLED(4);	// LED 4 invalid received
        }
    }
    else
    {
        // Programming error, interrupts other than the only enabled interrupt 'MsgReceived'
    	// (which on some platforms is radio DISABLED)
    	// FUTURE handle more gracefully by just clearing all events???
    	assert(false);
    }
    // We don't have a queue and we don't have a callback for idle
    assert(!isEventForMsgReceivedInterrupt());	// Ensure event is clear else get another unexpected interrupt
}



/*
 * Private routines that isolate which event is used for interrupt on End Of Transmission.
 * The choices are END or DISABLED.
 */
#ifdef PACKET_DONE_INTERRUPT
bool Radio::isEventForEOTInterrupt() { return device.isPacketDone(); }
void Radio::clearEventForEOTInterrupt() { device.clearPacketDoneEvent(); }
void Radio::enableInterruptForEOT() { device.enableInterruptForPacketDoneEvent(); }
void Radio::disableInterruptForEOT() { device.disableInterruptForPacketDoneEvent(); }
bool Radio::isEnabledInterruptForEOT() { return device.isEnabledInterruptForPacketDoneEvent(); }
#else
// DISABLED event
bool Radio::isEventForMsgReceivedInterrupt() {
	/*
	 * !!! The radio stays in the disabled state, even after the event is cleared.
	 * So this is not:  device.isDisabled().
	 */
	return device.isDisabledEventSet();
}
void Radio::clearEventForMsgReceivedInterrupt() { device.clearDisabledEvent(); }
void Radio::enableInterruptForMsgReceived() { device.enableInterruptForDisabledEvent(); }
void Radio::disableInterruptForMsgReceived() { device.disableInterruptForDisabledEvent(); }
void Radio::disableInterruptForEndTransmit() { device.disableInterruptForDisabledEvent(); }
bool Radio::isEnabledInterruptForMsgReceived() { return device.isEnabledInterruptForDisabledEvent(); }
bool Radio::isEnabledInterruptForEndTransmit() { return device.isEnabledInterruptForDisabledEvent(); }
#endif

/*
 * Packet was received, but it may be invalid
 */
bool Radio::isValidPacket(){
	// We don't sample RSSI
	// We don't use DAI device address match (which is a prefix of the payload)
	// We don't use RXMATCH to check which logical address was received
	// (assumed environment with few foreign 2.4Mhz devices.)
	// We do check CRC (messages destroyed by noise.)
	return device.isCRCValid();
}



void Radio::dispatchPacketCallback() {
	// Dispatch to owner callbacks
	if ( ! wasTransmitting ) { aRcvMsgCallback(); }
	// No callback for xmit
}



void Radio::init(void (*onRcvMsgCallback)()) {
	aRcvMsgCallback = onRcvMsgCallback;

	// Not require device power on
	powerSupply.enableDCDCPower();	// Radio device wants this enabled.

	// assert radio is configured to device reset defaults.
	// But this code doesn't handle all default events from radio,
	// and default configuration of radio is non-functional.
	// Caller must do additional configuration.

	// not ensure Radio or its underlying RadioDevice is functional.
	// not ensure isPowerOn()
}


// Configuration

// TODO rename Fixed and Variable
void Radio::configureStatic() {
	// Must be redone whenever radio device transitions from power off=>on?
	// None of it may be necessary if you are happy with reset defaults?

	// Specific to the protocol, here rawish
	device.configureFixedFrequency();
	device.configureFixedLogicalAddress();
	device.configureNetworkAddressPool();
	device.configureCRC();
	device.configureStaticPacketFormat(PayloadCount, NetworkAddressLength);
	device.setShortcutsAvoidSomeEvents();

	// Static: device always use single buffer owned by radio
	device.configurePacketAddress(staticBuffer);

	// FUTURE: parameters
	// Default tx power
	// Default mode i.e. bits per second
	assert(device.frequency() == 2);
}



// Powering
// Power off saves more power than just disable.  A SoftDevice powers off to save power.

void Radio::powerOnAndConfigure() {
	powerOn();
	configureStatic();
}


void Radio::powerOn() {
	assert(!device.isPowerOn());  // require off

	// require Vcc > 2.1V (see note below about DCDC)

	hfClock.startXtalSource();	// radio requires XTAL!!! hf clock, not the RC hf clock
	device.setRadioPowered(true);
	spinUntilReady();

	// assert if it was off, the radio and its registers are in initial state as specified by datasheet
	// i.e. it needs to be reconfigured

	// assert HFCLK is on since radio uses it

	// !!! Configuration was lost, caller must now configure it
	assert(!isEnabledInterruptForMsgReceived());
	assert(device.isDisabled());		// after power on and ready, initial state is DISABLED
}


void Radio::spinUntilReady() {
	// BUG: WAS disable(); spinUntilDisabled();  but then I added an assertion to disable();
	/*
	 * Wait until power supply ramps up.
	 * Until then, device registers are indeterminate?
	 *
	 * This is the way RadioHead does it.
	 * But how do you start a task when registers are indeterminate?
	 */
	device.clearDisabledEvent();
	device.startDisablingTask();
	spinUntilDisabled();
}

void Radio::powerOff() {
	// not require on; might be off already

	/*
	 * require disabled: caller must not power off without disabling
	 * because we also use the disabled state for MsgReceived (with interrupts)
	 * The docs do not make clear whether the device passes through
	 * the DISABLED state (generating an interrupt) when powered off.
	 */
	assert(device.isDisabled());

	device.setRadioPowered(false);
	// not ensure not ready; caller must spin if necessary

	hfClock.stopXtalSource();
	// assert hf RC clock resumes for other peripherals

	// assert radio and HFCLK are off, or will be soon
}


bool Radio::isPowerOn() { return device.isPowerOn(); }

#ifdef OBSOLETE
// Get address and length of buffer the radio owns.
void Radio::getBufferAddressAndLength(uint8_t** handle, uint8_t* lengthPtr) {
	*handle = staticBuffer;
	*lengthPtr = PayloadCount;
}
#endif

uint8_t* Radio::getBufferAddress() { return staticBuffer; }


void Radio::transmitStaticSynchronously(){
	disableInterruptForEndTransmit();	// spin, not interrupt
	transmitStatic();
	spinUntilDisabled();
	// assert xmit is complete and device is disabled
}

void Radio::transmitStatic(){
	wasTransmitting = true;
	startXmit();
	// not assert xmit is complete, i.e. asynchronous and non-blocking
};

void Radio::receiveStatic() {
	wasTransmitting = false;
	setupInterruptForMsgReceivedEvent();
	startRcv();
	// assert will get IRQ on message received
}

#ifdef DYNAMIC
void Radio::transmit(volatile uint8_t * data, uint8_t length){
	wasTransmitting = true;
	setupXmitOrRcv(data, length);
	startXmit();
	// not assert xmit is complete, i.e. asynchronous and non-blocking
};


void Radio::receive(volatile uint8_t * data, uint8_t length) {
	wasTransmitting = false;
	setupXmitOrRcv(data, length);
	startRcv();
}
#endif

/*
 * Enabling
 *
 * Enabling is final step.
 * Start a task on the device.
 * Require device is configured, including data.
 */

void Radio::enableRX() {
	device.clearMsgReceivedEvent();
	nvic.enableRadioIRQ();
	device.startRXTask();
}
void Radio::enableTX() {
	device.clearEndTransmitEvent();
	// No interrupt for xmit.
	device.startTXTask();
}

void Radio::disable() {
	assert(!isEnabledInterruptForEndTransmit());
	device.clearDisabledEvent();
	device.startDisablingTask();
}

bool Radio::isDisabled() { return device.isDisabled(); }

void Radio::spinUntilDisabled() {
	// Assert we started the task DISABLE or we think isDisabled, want to assert isDisabled
	// Wait until the event that signifies disable is complete
	// See data sheet.  For (1Mbit mode, TX) delay is ~6us, for RX, ~0us
	while (!device.isDisabled()) ;
}


// TODO this assertion is not a require, just how we happen to use it.
void Radio::setupInterruptForMsgReceivedEvent() {
	/*
	 * Assert
	 * is configured: shortcuts, packetAddress, etc.
	 * Fixed-length buffer is filled
	 */
	enableInterruptForMsgReceived();
	clearEventForMsgReceivedInterrupt();
}


#ifdef DYNAMIC
void Radio::setupXmitOrRcv(volatile uint8_t * data, uint8_t length) {
	/*
	 * Assert
	 * is configured: shortcuts, packetAddress, etc.
	 */
	device.setShortcutsAvoidSomeEvents();
	device.configurePacketAddress(data);
	//device.configurePacketLength(length);
	enableInterruptForEOT();
	clearEventForEOTInterrupt();
}
#endif





// task/event architecture, these trigger or enable radio device tasks.

void Radio::startXmit() {
	assert(device.isDisabled());  // require, else behaviour undefined per datasheet
	enableTX();
	// assert radio state will soon be TXRU and since shortcut, TX
}

void Radio::startRcv() {
	assert(device.isDisabled());  // require, else behaviour undefined per datasheet
	enableRX();
	// assert radio state will soon be RXRU, then since shortcut, RX
}

void Radio::stopReceive() {
	/*
	 *  assert radio state is:
	* RXRU : aborting before ramp-up complete
	* or RXIDLE: on but never received start preamble signal
	* or RX: in middle of receiving a packet
	* or DISABLED: message was received and RX not enabled again
	* */

	// In interrupt chain, disable in two places: nvic and device
	nvic.disableRadioIRQ();
	disableInterruptForMsgReceived();
	// TODO just device.disableInterruptForMsgReceived

	if (! device.isDisabled()) {
		// was receiving and no messages received (device in state RXRU, etc. but not in state DISABLED)
		device.startDisablingTask();
		// assert radio state soon RXDISABLE and then immediately transitions to DISABLED
		spinUntilDisabled();
	}
	assert(device.isDisabled());
	// assert no interrupts enabled from radio, for any event
}

void Radio::stopXmit() {
	// Rarely used: to abort a transmission, generally radio completes a xmit
	// FUTURE
}

void Radio::spinUntilXmitComplete() {
	// Spin mcu until xmit complete.
	// Alternatively, sleep, but radio current dominates mcu current anyway?
	// Xmit takes a few msec?

	//assert isTransmitting

	// Nothing can prevent xmit?  Even bad configuration or HFCLK not on?

	// Radio state flows (via TXDISABLE) to DISABLED.
	// Wait for DISABLED state (not the event.)
	// Here, for xmit, we do not enable interrupt on DISABLED event.
	// Since it we expect it to be quick.
	// FUTURE use interrupt on xmit.
	while (! device.isDisabled() ) {}
}





