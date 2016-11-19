
#include "powerManager.h"
#include "powerComparator.h"

namespace {

/*
 * Somewhat arbitrary choice to use this device: simpler and portable.
 */
PowerComparator powerComparator;

}  // namespace



/*
 * Specific to my application:
 *  - storage capacitor is certain joules.
 *  - power supply delivers about 10uA @2.4V (solar cell, in 200 lux.)
 *  - power supply can deliver 4.8V (solar cell in full sun)
 *  - mcu Vbrownout (aka Vmin) is 1.9V
 *  - mcu Vmax is 3.6V
 *
 * Need say 2.1V on capacitor to burst the radio without brownout.
 * Need say 2.3V on capacitor to burst the work without falling below 2.1V
 *
 */
bool PowerManager::isExcessVoltage() { return powerComparator.isVddGreaterThan2_7V();}
bool PowerManager::isPowerForWork() {return powerComparator.isVddGreaterThan2_3V();}
bool PowerManager::isPowerForRadio() { return powerComparator.isVddGreaterThan2_1V();}




