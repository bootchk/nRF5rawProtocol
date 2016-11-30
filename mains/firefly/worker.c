
#include "worker.h"

#include "platform/ledFlasher.h"


/*
 * A Worker implementation where work is: flash an LED
 */

namespace {

LEDFlasher ledFlasher;

unsigned int amount = 1;

}

void Worker::init() {
	ledFlasher.init();
	setLeastAmount();
}

void Worker::work() {
	ledFlasher.flashLEDByAmount(1, amount);
}

void Worker::setLeastAmount() {
	amount = 1;
}
void Worker::increaseAmount() {
	if (amount < LEDFlasher::MaxFlashAmount) amount++;
}
void Worker::decreaseAmount() {
	if (amount > 1) amount--;
}
void Worker::setAmountModerate() {
	amount = 20; 	// for 0.6mSec unit, gives 12mSec flash.
}

