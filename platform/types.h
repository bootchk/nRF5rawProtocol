#pragma once

#include <inttypes.h>

/*
 * Fundamental types between platform and SyncAgent.
 */



/*
 * Ticks delivered by platform's free-running, circular clock/counter.
 * Called 'OS' but the platform need not have a true OS.
 *
 * On some platforms, only lower 24 bits valid.
 */
// FUTURE implement enforcement in type of 24-bit limitation on some platforms.
// For now, as long as don't schedule more than 512 seconds ahead, should be OK.
typedef uint32_t OSTime;


/*
 * MAC id of radio, unique system/unit identifier.
 * Content of sync msg to identify  master.
 * BT: 48-bits
 */
typedef uint64_t SystemID;	// lower 6 btyes


/*
 * Work delivered by SyncAgent to and from app.
 *
 * Currently an instance of primitive type.
 *
 * !!! Must be same size as DeltaSync.
 * Constrained by current implementation of SyncAgent.
 */
typedef uint8_t WorkPayload;


/*
 * Pointer to unsigned bytes writeable concurrently by device and mcu.
 * The pointed-to uint8_t is volatile.
 */
typedef volatile uint8_t * BufferPointer;

