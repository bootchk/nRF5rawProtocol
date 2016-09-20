
#include <cassert>

// independent of nrf.h

#include "transport.h"

// Class (singleton) data members
Radio RawTransport::radio;
void (*RawTransport::aRcvMsgCallback)();


/*
 * State.
 * Can't tell from radio device whether xmit or rcv task was started,
 * (when using shortcuts.)
 */
bool RawTransport::wasTransmitting;



/*
 * IRQ Handler for radio
 *
 * Hack: really belongs to radio
 */
extern "C" {
void RADIO_IRQHandler() {
	RawTransport::eventHandler();	// relay
}
}


void RawTransport::eventHandler(void)
{
    if (radio.isPacketDone())
    {
        // We don't sample RSSI
    	// We don't use DAI device address match (which is a prefix of the payload)
    	// We don't use RXMATCH to check which logical address was received (assumed environment with few foreign 2.4Mhz radios.)
    	// We do check CRC (messages destroyed by noise.)

        radio.clearPacketDoneEvent();

        if (radio.isCRCValid()) dispatchPacketCallback();
        // else garbled message received, ignore it
    }
    else
    {
        // Programming error, interrupts other than the only enabled interrupt 'packet done'
    	// FUTURE handle more gracefully by just clearing
    	assert(false);
    }
    // We don't have a queue and we don't have a callback for idle
}


void RawTransport::dispatchPacketCallback() {
	// Dispatch to owner callbacks
	if ( wasTransmitting ) { aRcvMsgCallback(); }
	// No callback for xmit
}




// Xmit

void RawTransport::transmit(uint8_t * data){
	wasTransmitting = true;
	radio.setupXmitOrRcv(data);
	radio.startXmit();
};


// Rcv

void RawTransport::startReceiver(uint8_t * data) {
	wasTransmitting = false;
	radio.setupXmitOrRcv(data);
	radio.startRcv();
}

void RawTransport::init(void (*onRcvMsgCallback)()) {
	radio.init();
	aRcvMsgCallback = onRcvMsgCallback;
}


// Configuration

void RawTransport::configure() {
	// Should be redone whenever radio is power toggled on?
	// None of it may be necessary if you are happy with reset defaults?

	// Specific to the protocol, here rawish
	radio.configureFixedFrequency();
	radio.configureFixedLogicalAddress();
	radio.configureNetworkAddressPool();
	radio.configureCRC();
	radio.configurePacketFormat();

	// FUTURE: parameters
	// Default tx power
	// Default mode i.e. bits per second
};




// Pass through

void RawTransport::powerOn() { radio.powerOn(); }
void RawTransport::powerOff() { radio.powerOff(); }
bool RawTransport::isDisabled() { return radio.isDisabled(); }
void RawTransport::stopReceiver(){ radio.stopRcv(); }
void RawTransport::spinUntilXmitComplete() { radio.spinUntilXmitComplete(); }



