#pragma once


/*
 * The HF (16Mhz) xtal clock.
 * Required by radio.
 *
 * See a note on Nordic forums: https://devzone.nordicsemi.com/question/18096/what-is-the-high-frequency-clock-model-for-nrf51/
 * Briefly it says a HF RC clock is used by other parts of the system when the HF xtal clock is stopped.
 * And the HF RC clock starts and stops automatically when the HF xtal clock is stopped.
 * The radio needs a running HF xtal clock, but a wireless stack may stop it to save power.
 *
 * See comments in hfClock.c.
 */

class HfClock {
public:
	static void startXtalSource();
	static void stopXtalSource();
};
