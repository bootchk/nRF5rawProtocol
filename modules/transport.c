
#include <cassert>

// independent of nrf.h

#include "transport.h"

// Class (singleton) data members
Radio RawTransport::radio;




/*
 * IRQ Handler
 */
void RawTransport::eventHandler(void)
{
    if (radio.isPacketDone())
    {
        // We don't sample RSSI
    	// We don't use DAI device address match (which is a prefix of the payload)
    	// We don't use RXMATCH to check which logical address was received (assumed environment with few foreign 2.4Mhz radios.)
    	// We do check CRC (messages destroyed by noise.)

        radio.clearPacketDoneFlag();

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
	// Dispatch to Radio owner callbacks
	if ( radio.wasTransmitting() )
	{
		// TODO receiveCallback()
	}
	else
	{
		// TODO transmitCallback()
	}
}




// Xmit

void RawTransport::transmit(void * data){
	radio.setupXmitOrRcv(data);
	radio.startXmit();
};



void RawTransport::startReceiver() {
	int data;	// TODO allocate buffers

	radio.setupXmitOrRcv(&data);
	radio.startRcv();
}



// Pass through

void RawTransport::init() { radio.init(); }
void RawTransport::configure() { radio.configureAfterPowerOn(); }
void RawTransport::powerOn() { radio.powerOn(); }
void RawTransport::powerOff() { radio.powerOff(); }
bool RawTransport::isDisabled() { return radio.isDisabled(); }
void RawTransport::stopReceiver(){ radio.stopRcv(); }
void RawTransport::spinUntilXmitComplete() { radio.spinUntilXmitComplete(); }


