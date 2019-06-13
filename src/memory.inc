//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// N E X T   M E M O R Y   S Y S T E M
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// NEXT MEMORY SYSTEM
//----------------------------------------------------------------------------------------------------------------------
//
// Memory Map:
//
//      Real addresses      Size        8K bank     Description
//      ---------------     ----        -------     -----------------------
//      $000000-$00ffff     64K                     ZX Spectrum ROM             UNEXPANDED
//      $010000-$013fff     16K                     EsxDOS ROM
//      $014000-$017fff     16K                     MF ROM
//      $018000-$01bfff     16K                     MF EXTRA ROM
//      $01c000-$01ffff     16K                     MF RAM
//      $020000-$03ffff     128K                    DivMMC RAM
//      $040000-$05ffff     128K        0-15        Standard 128K RAM
//      $060000-$07ffff     128K        16-31       Extra RAM
//      $080000-$0fffff     512K        32-95       1st extra IC RAM
//      -------------------------------------------------------------------
//      $100000-$17ffff     512K        96-159      2nd extra IC RAM            EXPANDED
//      $180000-$1fffff     512K        160-223     3rd extra IC RAM
//
// Total memory = $200000 (2MB)
//
// Memory decode order:
//
//      $0000-$3fff     Boot ROM
//                      Machine config mapping
//                      Multiface
//                      DivMMC
//                      Layer 2 mapping
//                      MMU
//                      ROMCS expansion bus
//                      ROM
//
//      $4000-$bfff     Layer 2 mapping
//                      MMU
//
//      $c000-$ffff     MMU mapping
//      
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

void nxMemoryInit(MemoryPtr mem)
{
    arrayResize(mem->bytes, K_MB(2));
    memoryCopy(nextrom_data, mem->bytes + 0x000000, K_KB(64));
    memoryCopy(nextmmc_data, mem->bytes + 0x010000, K_KB(8));

    nxMemSlot(mem, 0, MEMTYPE_ZXRom, 0);
    nxMemSlot(mem, 1, MEMTYPE_ZXRom, 1);
    nxMemSlot(mem, 2, MEMTYPE_RAM, 10);
    nxMemSlot(mem, 3, MEMTYPE_RAM, 11);
    nxMemSlot(mem, 4, MEMTYPE_RAM, 4);
    nxMemSlot(mem, 5, MEMTYPE_RAM, 5);
    nxMemSlot(mem, 6, MEMTYPE_RAM, 0);
    nxMemSlot(mem, 7, MEMTYPE_RAM, 1);
}

//----------------------------------------------------------------------------------------------------------------------

void nxMemoryDone(MemoryPtr mem)
{
    arrayDone(mem->bytes);
}

//----------------------------------------------------------------------------------------------------------------------

void nxMemSlot(MemoryPtr mem, int slot, MemType type, int page)
{
    K_ASSERT(K_INRANGE(slot, 0, 8));

    i64 offset = 0;
    int maxPages = 0;
    bool readonly = NO;

    switch(type)
    {
    case MEMTYPE_ZXRom:     offset = 0x000000; maxPages = 8;    readonly = YES;                 break;
    case MEMTYPE_ESXDos:    offset = 0x010000; maxPages = 2;    readonly = YES;                 break;
    case MEMTYPE_Multiface: offset = 0x014000; maxPages = 6;    readonly = K_BOOL(page < 4);    break;
    case MEMTYPE_DivMMC:    offset = 0x020000; maxPages = 16;                                   break;
    case MEMTYPE_RAM:       offset = 0x040000; maxPages = 224;                                  break;
    default:
        K_ASSERT(0);
        offset = 0;
    }

    K_ASSERT(K_INRANGE(page, 0, 224));
    mem->slots[slot].offset = offset + (page * K_KB(8));
    mem->slots[slot].readonly = readonly;
}

//----------------------------------------------------------------------------------------------------------------------

