
#pragma once

#include <inttypes.h>

/*
 * Simple mailbox:
 * - holding ints
 * - listener polls
 * - not thread-safe (only one poster and listener)
 *
 * Statically configured to empty.
 * Algebra:
 * reset; isMail() == false
 *
 * Note there is no init() to empty the mailbox.
 * After an exception, it might be necessary to
 *
 */



class Mailbox {
public:
	// put overwrites a mail when overflows
	static void put(uint32_t item);

	// fetch first mail in box (if queued)
	static uint32_t fetch();

	//
	static bool isMail();
};
