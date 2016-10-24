
#pragma once

#include <inttypes.h>

/*
 * Simple mailbox:
 * - holding ints
 * - listener polls
 * - not thread-safe (only one poster and listener)
 */



class Mailbox {
public:
	static void put(uint32_t item);
	static uint32_t fetch();
	static bool isMail();
};
