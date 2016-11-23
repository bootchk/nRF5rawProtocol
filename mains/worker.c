
#include "worker.h"

#include "platform/ledFlasher.h"


/*
 * A Worker implementation where work is: flash an LED
 */

namespace {

LEDFlasher ledFlasher;

}

void Worker::init() {
	ledFlasher.init();
}

void Worker::work() {

}

void Worker::setLeastAmount() {

}
void Worker::increaseAmount() {

}
void Worker::decreaseAmount() {

}

