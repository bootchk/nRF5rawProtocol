
#include <cassert>

// independent of nrf.h

#include "transport.h"

// Class (singleton) data members
RadioDevice RawTransport::device;
Radio RawTransport::radio;
void (*RawTransport::aRcvMsgCallback)();





// TODO Hack: really belongs to radio
extern "C" {
void RADIO_IRQHandler() {
	RawTransport::eventHandler();	// relay
}
}


void RawTransport::eventHandler(void)
{
    if (device.isPacketDone())
    {
        // We don't sample RSSI
    	// We don't use DAI device address match (which is a prefix of the payload)
    	// We don't use RXMATCH to check which logical address was received
    	// (assumed environment with few foreign 2.4Mhz devices.)
    	// We do check CRC (messages destroyed by noise.)

        device.clearPacketDoneEvent();

        if (device.isCRCValid()) dispatchPacketCallback();
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







void RawTransport::init(void (*onRcvMsgCallback)()) {
	radio.init();
	aRcvMsgCallback = onRcvMsgCallback;
}







// Pass through

/*
void RawTransport::powerOn() { radio.powerOn(); }
void RawTransport::powerOff() { radio.powerOff(); }
bool RawTransport::isDisabled() { return radio.isDisabled(); }
void RawTransport::stopReceiver(){ radio.stopRcv(); }
void RawTransport::spinUntilXmitComplete() { radio.spinUntilXmitComplete(); }
*/


