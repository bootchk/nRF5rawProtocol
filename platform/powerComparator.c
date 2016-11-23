
#include <cassert>
#include "nrf.h"

#include "powerComparator.h"

/*
 * Implementation notes:
 * not using an interrupt
 */

namespace {

/*
 * Set threshold .
 * Side effect is disable.
 * Device will not compare until enabled.
 *
 * Alternative is to set threshold without clearing disable bit, using mask POWER_POFCON_POF_Msk
 */
void setThreshold2_1AndDisable() {
	/*
	 * Set and clear multiple bits.
	 */
	NRF_POWER->POFCON = POWER_POFCON_THRESHOLD_V21 << POWER_POFCON_THRESHOLD_Pos;
}
void setThreshold2_3AndDisable() { NRF_POWER->POFCON = POWER_POFCON_THRESHOLD_V23 << POWER_POFCON_THRESHOLD_Pos; }
void setThreshold2_5AndDisable() { NRF_POWER->POFCON = POWER_POFCON_THRESHOLD_V25 << POWER_POFCON_THRESHOLD_Pos; }
void setThreshold2_7AndDisable() { NRF_POWER->POFCON = POWER_POFCON_THRESHOLD_V27 << POWER_POFCON_THRESHOLD_Pos; }

/*
 * Did device generate event for the configured threshold?
 * Does not clear the event.
 */
bool isPOFEvent() { return NRF_POWER->EVENTS_POFWARN == 1; }

void clearPOFEvent() { NRF_POWER->EVENTS_POFWARN = 0; }

/*
 * Return result of compare Vdd to threshold.
 * Leaves:
 * - device disabled
 * - event cleared
 *
 */
bool testVddThenDisable() {
	bool result;

	PowerComparator::enable();
	/*
	 * Event indicates Vdd less than threshold.
	 * Invert: return true if Vdd greater than threshold.
	 */
	result = ! isPOFEvent();
	PowerComparator::disable();
	clearPOFEvent();
	return result;
}

}  // namespace





void PowerComparator::enable() {
	/*
	 * BIS bitset the enable bit
	 */
	NRF_POWER->POFCON |= POWER_POFCON_POF_Msk;
	/*
	 * !!! Flush ARM write cache to ensure immediate effect
	 */
	(void) NRF_POWER->POFCON;

	assert( NRF_POWER->POFCON & POWER_POFCON_POF_Msk);
	// assert event is set if power is less than threshold
}

void PowerComparator::disable() {
	/*
	 * BIC bitclear the enable bit
	 */
	NRF_POWER->POFCON &= ! POWER_POFCON_POF_Msk;
	/*
	 * !!! Flush ARM write cache to ensure immediate effect
	 */
	(void) NRF_POWER->POFCON;

	assert( ! (NRF_POWER->POFCON & POWER_POFCON_POF_Msk) );
}



/*
 * !!! Implementation is correct even if other interrupts from same source are used.
 * In INTCLR, only a 1-bit clears an interrupt source, a 0-bit has no effect on interrupts.
 */
void PowerComparator::disableInterrupt() {
	NRF_POWER->INTENCLR = 1 << POWER_INTENCLR_POFWARN_Pos;
}

bool PowerComparator::isVddGreaterThan2_1V() {

	// FUTURE Ensure our bit twiddling not generate interrupts
	// disableInterrupt();
	// Assume default state of interrupt not enabled.
	// If caller has enabled interrupts, caller must handle them.

	setThreshold2_1AndDisable();
	return testVddThenDisable();
}

bool PowerComparator::isVddGreaterThan2_3V(){
	setThreshold2_3AndDisable();
	return testVddThenDisable();
}
bool PowerComparator::isVddGreaterThan2_5V(){
	setThreshold2_5AndDisable();
	return testVddThenDisable();
}
bool PowerComparator::isVddGreaterThan2_7V(){
	setThreshold2_7AndDisable();
	return testVddThenDisable();
}
