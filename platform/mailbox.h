
#pragma once

#include "types.h"	// local types

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
	static void put(WorkPayload item);

	// fetch first mail in box (if queued)
	static WorkPayload fetch();

	//
	static bool isMail();
};
