
#include <cassert>

// independent of nrf.h

#include "transport.h"

// Class (singleton) data members
Radio RawTransport::radio;
void (*RawTransport::aRcvMsgCallback)();
bool RawTransport::wasTransmitting;	// state



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
        // Unexpected, interrupts other than 'packet done'
    	// TODO log
    	// TODO clear flags
    }
    // We don't have a queue and we don't have a callback for idle
}


void RawTransport::dispatchPacketCallback() {
	// Dispatch to owner callbacks
	if ( wasTransmitting ) { aRcvMsgCallback(); }
	// No callback for xmit
}




// Xmit

void RawTransport::transmit(void * data){
	wasTransmitting = true;
	radio.setupXmitOrRcv(data);
	radio.startXmit();
};


// Rcv

void RawTransport::startReceiver() {
	int data;	// TODO allocate buffers

	wasTransmitting = false;
	radio.setupXmitOrRcv(&data);
	radio.startRcv();
}

void RawTransport::init(void (*onRcvMsgCallback)()) {
	radio.init();
	aRcvMsgCallback = onRcvMsgCallback;
}

// Pass through

void RawTransport::configure() { radio.configureAfterPowerOn(); }
void RawTransport::powerOn() { radio.powerOn(); }
void RawTransport::powerOff() { radio.powerOff(); }
bool RawTransport::isDisabled() { return radio.isDisabled(); }
void RawTransport::stopReceiver(){ radio.stopRcv(); }
void RawTransport::spinUntilXmitComplete() { radio.spinUntilXmitComplete(); }



