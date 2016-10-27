
#include <cassert>

#include "mailbox.h"


// This implementation holds only one item


// static data members
namespace {

uint32_t item;
bool isItem = false;

}

void Mailbox::put(uint32_t aItem){
	// TODO Thread safe: atomic
	item = aItem;
	isItem = true;
}

uint32_t Mailbox::fetch(){
	assert(isItem);
	// Thread safe: copy item before deleting from queue
	uint32_t result = item;
	isItem = false;
	return result;
}

bool Mailbox::isMail(){
	return isItem;
}
