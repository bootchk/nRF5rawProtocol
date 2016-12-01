

#include "workSupervisor.h"
#include "worker.h"
#include "groupWork.h"

#include "platform/powerManager.h"


/*
 * Implementation notes:
 *
 * Self uses PowerManager to adjust work on Worker.
 *
 * Self manages the amount of work on Worker,
 * but GroupWork tells Worker to work.
 */
namespace {

PowerManager powerManager;
Worker worker;
GroupWork groupWork;

/*
 * A very simple approach, not very effective?
 */
// TODO replace this with a state machine that is more effective

void simpleManagePowerWithWork() {
	// Exclusive cases
	if (powerManager.isVoltageExcess()) {
		// e.g. > 2.7V
		/*
		 * Self MUST work locally to keep voltage from exceeding Vmax
		 * Not sent to group.
		 */
		worker.increaseAmount();
		groupWork.workInIsolation();
	};

	if (powerManager.isVoltageHigh()) {
		// e.g. 2.5V - 2.7V
		/*
		 * I could work.
		 */
		worker.decreaseAmount();
		groupWork.randomlyInitiateGroupWork();
	}

	if (powerManager.isVoltageMedium()) {
		// e.g. 2.3-2.5V
		/*
		 *  not change amount of work
		 *
		 *  not initiate work.
		 *  Others may ask me to work, see elsewhere.
		 */
	};

	if (powerManager.isVoltageLow()) {
		// e.g. 2.1-2.3V

		/*
		 * Not enough power to work.
		 * Next work done (if ever) is least amount.
		 */
		worker.setLeastAmount();
	}
	// One more range we don't use: isVoltageUltraLow()
}

} // namespace



void WorkSupervisor::init(Mailbox* aMailbox) {
	worker.init();

	// self doesn't use mailbox, merely passes mailbox to groupWorker
	// self owns worker, but groupWorker also uses it
	groupWork.init(aMailbox, &worker);

	// PowerManager needs no init
}



/*
 * Part of the total voltage/power management.
 * This is high side.
 * See also SyncSleep, which also does power managment on the low side.
 *
 * Here, if voltage is climbing, do more work.
 * Note work is asynchronous, more work does not require more time in this routine.
 *
 * This is at syncpoint (but it may be drifted.)
 *
 * This also determines the behaviour of the flashing:
 * - units try to flash in sync
 * - units with excess power may flash when other units can't
 * - when all units have moderate power, some unit may randomly trigger all to flash
 *
 */
void WorkSupervisor::manageVoltageByWork() {

/*
 * Peculiar to my test setup:
 *
 * one unit is a nRF52DK, with regulated, line power, thus there is always enough power
 * and never too much: no need to manage voltage.
 *
 * The other units are nRF51 with unregulated solar power.
 * Depending on the light, the voltage may exceed Vmax 3.6V of the unit,
 * so this algorithm attempts to use power to control voltage.
 *
 * (The solar cells can produce a max of 4.8V, which is not exceedingly dangerous to the chips.)
 */
#ifdef NRF52
	// amount of work is enough to perceive
	groupWork.randomlyInitiateGroupWork();
#else
	simpleManagePowerWithWork();
#endif


}


void WorkSupervisor::tryWorkInIsolation() {
	if (powerManager.isPowerForWork()) {
		// do work at the managed amount
		groupWork.workInIsolation();
	}
	// else omit work, not enough power
}


void WorkSupervisor::workInIsolation() {

	// do work at the managed amount
	groupWork.workInIsolation();
}


