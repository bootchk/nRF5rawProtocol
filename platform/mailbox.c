
#include <cassert>

#include "mailbox.h"


/*
 * This implementation:
 * - holds only one item.
 * - is not generic on type of object held
 * - is not thread safe
 */


// static data members
namespace {

WorkPayload item;
bool isItem = false;

}

void Mailbox::put(WorkPayload aItem){
	// FUTURE Thread safe: atomic
	item = aItem;
	isItem = true;
}

WorkPayload Mailbox::fetch(){
	assert(isItem);
	// Thread safe: copy item before deleting from queue
	WorkPayload result = item;
	isItem = false;
	return result;
}

bool Mailbox::isMail(){
	return isItem;
}
