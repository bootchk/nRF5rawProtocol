#pragma once


/*
 * Abstracts work, i.e. the implementation defines what "work" is.
 *
 * Typically:
 * - flashing an LED
 * - some other transducer, sensing or driving
 *
 * Amount of work is settable.
 * Units are defined in the implementation (typically units of 0.6mSec LED flash.)
 * So that work can be used to shed power.
 */


class Worker {
public:
	static void init();
	static void work();

	static void setLeastAmount();
	static void increaseAmount();
	static void decreaseAmount();

	/*
	 * Amount enough to easily see, when power is line power and regulated.
	 */
	static void setAmountModerate();
};
