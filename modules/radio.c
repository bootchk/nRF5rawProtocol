
#include "nrf.h"

#include "radio.h"

void Radio::configure() {

	// Freq is one of 3 BT advertising channels
	NRF_RADIO->FREQUENCY = 2;	// or 26, 80

	// Whitening depends on freq?
	NRF_RADIO->DATAWHITEIV = 37 & RADIO_DATAWHITEIV_DATAWHITEIV_Msk;
};

/*
 * static void setup_event(radio_event_t* p_evt)
{
    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
    radio_channel_set(p_evt->channel);
    NRF_RADIO->PACKETPTR = (uint32_t) p_evt->packet_ptr;
    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->PREFIX1	= ((m_alt_aa >> 24) & 0x000000FF);
    NRF_RADIO->BASE1    = ((m_alt_aa <<  8) & 0xFFFFFF00);

    if (p_evt->event_type == RADIO_EVENT_TYPE_TX)
    {
        DEBUG_RADIO_SET_STATE(PIN_RADIO_STATE_TX);
        NRF_RADIO->TXADDRESS = p_evt->access_address;
        NRF_RADIO->TASKS_TXEN = 1;
        m_radio_state = RADIO_STATE_TX;
        NRF_RADIO->TXPOWER  = p_evt->tx_power;
    }
    else
    {
        DEBUG_RADIO_SET_STATE(PIN_RADIO_STATE_RX);
        if (m_alt_aa != RADIO_DEFAULT_ADDRESS)
        {
            // only enable alt-addr if it's different
            NRF_RADIO->RXADDRESSES = 0x03;
        }
        else
        {
            NRF_RADIO->RXADDRESSES = 0x01;
        }
        NRF_RADIO->TASKS_RXEN = 1;
        m_radio_state = RADIO_STATE_RX;
    }
}
*/

