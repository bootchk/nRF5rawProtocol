
#pragma once

#ifdef FUTURE
/*
 * !!! hack.
 *
 * If you are just building this project, use:
 * #include "boards.h"
 * If you are wedging another library into this project,
 * and that other library is #include ledLogger.h, it needs these hardcode paths.
 */
#include "/home/bootch/nrf5_sdk/components/toolchain/cmsis/"
#include "/home/bootch/nrf5_sdk/components/toolchain/cmsis/include/core_cm4.h"
#include "/home/bootch/nrf5_sdk/components/device/nrf.h"
#include "/home/bootch/nrf5_sdk/components/drivers_nrf/hal/nrf_gpio.h"
#include "/home/bootch/nrf5_sdk/examples/bsp/boards.h"	// via nested includes, depends on -DBOARD_<FOO> , defines LEDS_NUMBER and LEDS_LIST
#else
#include "boards.h"
#endif
/*
 * Log to set of LED's.
 *
 * Safe to log to any index, they just won't light if they don't exist.
 *
 * Safe to have more than one instance.
 * E.G. a library can have its own instance,
 * but of course you must coordinate use of LEDs.
 */
class LEDLogger {
private:
	static const uint8_t leds_list[LEDS_NUMBER];

public:
	static void init();
	static void toggleLEDs();
	static void toggleLED(int ordinal);
};
