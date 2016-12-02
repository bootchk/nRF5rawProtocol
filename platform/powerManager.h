
#pragma once

/*
 * Four levels gives five ranges.
 */
enum class VoltageRange { Excess, High, Medium, Low, UltraLow };

/*
 * Understands:
 * - apps power requirements
 * - systems power supply (not a battery, a capacitor.)
 *
 * The levels in descending order of power required:
 * - work
 * - sync keeping (radio and clock)
 * - sync maintenance (clock only)
 *
 *
 * For example all the above are duty-cycled (bursts of):
 * - work:  100 mA motor or 20mA LED
 * - sync keeping:  4mA radi0
 * - sync maintenance: mcu
 * The base is the RTC of 1.5uA, which always runs to burst the mcu, etc.
 * If there is not enough power to maintain Vmin for the mcu, brownout and reset occurs.
 *
 * Depends on some device that read system Vcc (Vdd).
 * Voltage levels on some charge storage (capacitor) indicates power level.
 *
 * Also understand excessVoltage level.
 * The power supply may be unregulated (solar)
 * and capable of system damaging voltages (e.g. 4.8V exceeding mcu Vmax of 3.6V.)
 */
class PowerManager {

public:
	/*
	 * Levels
	 */
	static bool isPowerExcess();
	static bool isPowerForReserve();
	static bool isPowerForWork();
	static bool isPowerForRadio();

	/*
	 * Ranges
	 */
	static VoltageRange getVoltageRange();
};
