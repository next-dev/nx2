//----------------------------------------------------------------------------------------------------------------------
// Nx types and declarations
//----------------------------------------------------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------------------------------------------------------------
// INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#define K_IMPLEMENTATION
#include <kore/kore.h>
#include <kore/kui.h>

#include "config.h"

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// N E X T   M E M O R Y   S Y S T E M
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//
// Represents one of the eight 8K memory slots visible by the Z80 CPU
//
typedef struct
{
    i64     offset;         // Memory offset
    bool    readonly;       // True if ROM
}
MemSlot;

//
// The memory system of the Next
//
typedef struct
{
    Array(u8)   bytes;          // The memory bytes
    MemSlot     slots[8];       // The slots
}
Memory, *MemoryPtr;

//
// API
//

void nxMemoryInit(MemoryPtr mem);
void nxMemoryDone(MemoryPtr mem);

typedef enum
{
    MEMTYPE_ZXRom,              // ZX Spectrum +3 ROM
    MEMTYPE_ESXDos,             // ESXDos ROM
    MEMTYPE_Multiface,          // Multiface memory
    MEMTYPE_DivMMC,             // DivMMC memory
    MEMTYPE_RAM,                // Normal pageable memory
}
MemType;

void nxMemSlot(MemoryPtr mem, int slot, MemType type, int page);