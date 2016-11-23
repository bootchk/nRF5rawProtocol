/*
 * Driver for nrf5x family PowerFailureComparator (POFCON) device.
 *
 * This is portable across nrf51 and nrf52 families.
 * (Unlike the SAADC.)
 *
 * nrf52 supports other discrete values e.g. 2_0.
 * Not implemented here since not portable.
 *
 * Functions return value which is discrete (bool), not continuous (float).
 * If you need a float type measurement, use SAADC.
 *
 * Not to be confused with any other comparator:
 * - COMP
 * - LPCOMP
 *
 * This driver is probably not compatible with other uses of the PowerFailComparator device:
 * to interrupt when power is failing to give the OS time to gracefully shut down.
 * There you are interested  being asynchronously notified of falling edges.
 * Here we are interested in synchronously the condition: above a given level.
 */

class PowerComparator {
public:
	/*
	 * These should probably be more hidden.
	 * For now, they are used only internally.
	 */
	static void enable();
	static void disable();
	static void disableInterrupt();

public:
	/*
	 * Most useful API.
	 *
	 * Simple tests of Vdd thresholds.
	 *
	 * These have side effects on the PowerFailureComparator device resource:
	 * they leave it disabled.
	 */
	// These are portable nrf51 and nrf52
	static bool isVddGreaterThan2_1V();
	static bool isVddGreaterThan2_3V();
	static bool isVddGreaterThan2_5V();
	static bool isVddGreaterThan2_7V();
};
