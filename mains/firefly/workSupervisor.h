
#pragma once

#include "../../platform/mailbox.h"


/*
 * Knows:
 * - work amount manages system voltage (loosely called power.)
 * - work is group work
 */

class WorkSupervisor {
public:
	static void init(Mailbox*);

	/*
	 * Initiate group work that also manages power.
	 */
	static void manageVoltageByWork();

	/*
	 * Work on this unit if power allows.
	 */
	static void tryWorkInIsolation();

	/*
	 * Definitely work on this unit to shed power.
	 */
	static void workInIsolation();
};
