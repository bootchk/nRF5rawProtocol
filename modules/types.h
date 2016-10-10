
#pragma once


/*
 * Basic types
 */

// Pointer to unsigned bytes writeable concurrently by device and mcu
// The pointed-to uint8_t is volatile.

typedef volatile uint8_t * BufferPointer;
