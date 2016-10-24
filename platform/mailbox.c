
#include <cassert>

#include "mailbox.h"


// This implementation holds only one item


// static data members
namespace {
uint32_t item;
bool isItem;
}

void Mailbox::put(uint32_t aItem){
	item = aItem;
	isItem = true;
}

uint32_t Mailbox::fetch(){
	assert(isItem);
	isItem = false;
	return item;
}

bool Mailbox::isMail(){
	return isItem;
}
