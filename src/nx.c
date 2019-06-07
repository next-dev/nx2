#include <Windows.h>

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// I N D E X
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
// Search for {KEYWORD}, where KEYWORD is one of the section labels below.
//
// ARRAY        C arrays and basic memory management.
// ENTRY        Main entry point for program
// MEMORY       Memory system of the Next.
// STATE        Emulator state.
//
//----------------------------------------------------------------------------------------------------------------------

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// B A S I C   T Y P E S
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef float    f64;

typedef char     bool;

typedef const char*     cstr;
typedef char*           str;

#define YES (1)
#define NO (0)
#define BOOL(b) ((b) ? YES : NO)

#define KB(x) (1024 * (x))
#define MB(x) (1024 * KB(x))
#define GB(x) (1024 * MB(x))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define ASSERT(x, ...) assert(x)

#define ZERO(d) memset(&(d), 0, sizeof(d))

#define BITCAST(type, value) (*(type *)&(value))

//----------------------------------------------------------------------------------------------------------------------{ARRAY}
//----------------------------------------------------------------------------------------------------------------------
// B A S I C   M E M O R Y   M A N A G E M E N T   A N D   A R R A Y S
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

// Basic memory allocation routines
void*   memoryAlloc         (i64 numBytes, cstr file, int line);
void*   memoryAllocClear    (i64 numBytes, cstr file, int line);
void*   memoryRealloc       (void* address, i64 oldNumBytes, i64 newNumBytes, cstr file, int line);
void    memoryFree          (void* address, i64 numBytes, cstr file, int line);

// Basic memory manipulation routines
void    memoryCopy          (const void* src, void* dst, i64 numBytes);
void    memoryMove          (const void* src, void* dst, i64 numBytes);
int     memoryCompare       (const void* mem1, const void* mem2, i64 numBytes);
void    memoryClear         (void* mem, i64 numBytes);

#define ALLOC(numBytes)                             memoryAlloc((numBytes), __FILE__, __LINE__)
#define CALLOC(numBytes)                            memoryAllocClear((numBytes), __FILE__, __LINE__)
#define REALLOC(address, oldNumBytes, newNumBytes)  memoryRealloc((address), (oldNumBytes), (newNumBytes), __FILE__, __LINE__)
#define FREE(address, oldNumBytes)                  memoryFree((address), (oldNumBytes), __FILE__, __LINE__)

//
// Arena API
//

#define ARENA_INCREMENT     4096
#define ARENA_ALIGN         16

typedef struct
{
    u8*     start;      // First byte of arena.
    u8*     end;        // One byte past the end of the arena.
    i64     cursor;     // Offset into arena for next allocation.
    i64     restore;    // Last position in the arena store/restore stack.
}
Arena, *ArenaPtr;

// Basic arena API
void    arenaInit           (ArenaPtr arena, i64 initialSize);
void    arenaDone           (ArenaPtr arena);
void*   arenaAlloc          (ArenaPtr arena, i64 numBytes);
void*   arenaAlign          (ArenaPtr arena);
void*   arenaAlignedAlloc   (ArenaPtr arena, i64 numBytes);

// Store/restore arena stack.
void    arenaPush           (ArenaPtr arena);
void    arenaPop            (ArenaPtr arena);

// Arena utilties
i64     arenaSpace          (ArenaPtr arena);
cstr    arenaFormatV        (ArenaPtr arena, cstr format, va_list args);
cstr    arenaFormat         (ArenaPtr arena, cstr format, ...);

//
// Array API
//

#define Array(T)            T*

// Destroy an array.
#define arrayDone(a) ((a) = ((a) ? FREE((u8 *)a - (sizeof(i64) * 2), (sizeof(*a) * __arrayCapacity(a)) + (sizeof(i64) * 2)), (void *)0 : (void *)0))

// Add an element to the end of an array and return the value.
#define arrayAdd(a, v) (__arrayMayGrow(a, 1), (a)[__arrayCount(a)++] = (v))

// Expand the array by 1 at the end and return the address.
#define arrayNew(a) arrayExpand((a), 1)

// Return the number of elements in an array.
#define arrayCount(a) ((a) ? __arrayCount(a) : 0)

// Returns a pointer past the end of the array.  Will be invalidated if you change the array.
#define arrayEnd(a) ((a) + __arrayCount(a))

// Add n uninitialised elements to the array and return the address to the new elements.
#define arrayExpand(a, n) (__arrayMayGrow(a, n), __arrayCount(a) += (n), &(a)[__arrayCount(a) - (n)])

// Enlarge an array proportional to its size.
#define arrayEnlarge(a) (arrayExpand((a), (arrayCount(a) / 2)))

// Set the final size of the array a to n
#define arrayResize(a, n) (((a) == 0 || __arrayCount(a) < (n)) ? arrayExpand(a, (n) - ((a) ? __arrayCount(a) : 0)) : ((__arrayCount(a) = (n)), (a)))

// Reserve capacity for n extra items to the array.
#define arrayReserve(a, n) (__arrayMayGrow(a, n))

// Clear the array.
#define arrayClear(a) ((a) ? (__arrayCount(a) = 0) : 0)

// Delete an array entry.
#define arrayDelete(a, i) (memoryMove(&(a)[(i)+1], &(a)[(i)], (__arrayCount(a) - (i) - 1) * sizeof(*(a))), --__arrayCount(a), (a))

// Insert an element.
#define arrayInsert(a, i) (arrayExpand((a),1), memoryMove(&(a)[(i)], &(a)[(i)+1], (__arrayCount(a) - (i) - 1) * sizeof(*(a))), (a) + (i))

// Index of element.
#define arrayIndexOf(a, e) (((e) - (a)) / sizeof(*(a)))

// Loop through an array an act on the elements.  Use: arrayFor(a) { a[i] = ... }
#define arrayFor(a) for (int i = 0; i < arrayCount(a); ++i)

// Array swap
#define arraySwap(a, i1, i2) (*arrayNew(a) = (a)[i1], (a)[i1] = (a)[i2], (a)[i2] = (a)[arrayCount(a)-1], --__arrayCount(a))

//
// Internal routines
//

#define __arrayRaw(a) ((i64 *)(a) - 2)
#define __arrayCount(a) __arrayRaw(a)[1]
#define __arrayCapacity(a) __arrayRaw(a)[0]

#define __arrayNeedsToGrow(a, n) ((a) == 0 || __arrayCount(a) + (n) > __arrayCapacity(a))
#define __arrayMayGrow(a, n) (__arrayNeedsToGrow(a, (n)) ? __arrayGrow(a, n) : 0)
#define __arrayGrow(a, n) ((a) = __arrayInternalGrow((a), (n), sizeof(*(a))))

void* __arrayInternalGrow(void* a, i64 increment, i64 elemSize);

//----------------------------------------------------------------------------------------------------------------------

static void* memoryOp(void* oldAddress, i64 oldNumBytes, i64 newNumBytes, cstr file, int line)
{
    void* p = 0;
    if (newNumBytes)
    {
        p = realloc(oldAddress, newNumBytes);
    }
    else
    {
        free(p);
    }

    return p;
}

//----------------------------------------------------------------------------------------------------------------------

void* memoryAlloc(i64 numBytes, cstr file, int line)
{
    return memoryOp(0, 0, numBytes, file, line);
}

//----------------------------------------------------------------------------------------------------------------------

void* memoryAllocClear(i64 numBytes, cstr file, int line)
{
    void* p = memoryOp(0, 0, numBytes, file, line);
    memoryClear(p, numBytes);
    return p;
}

//----------------------------------------------------------------------------------------------------------------------

void* memoryRealloc(void* address, i64 oldNumBytes, i64 newNumBytes, cstr file, int line)
{
    return memoryOp(address, oldNumBytes, newNumBytes, file, line);
}

//----------------------------------------------------------------------------------------------------------------------

void memoryFree(void* address, i64 numBytes, cstr file, int line)
{
    memoryOp(address, numBytes, 0, file, line);
}

//----------------------------------------------------------------------------------------------------------------------

void memoryCopy(const void* src, void* dst, i64 numBytes)
{
    memcpy(dst, src, (size_t)numBytes);
}

//----------------------------------------------------------------------------------------------------------------------

void memoryMove(const void* src, void* dst, i64 numBytes)
{
    memmove(dst, src, (size_t)numBytes);
}

//----------------------------------------------------------------------------------------------------------------------

int memoryCompare(const void* mem1, const void* mem2, i64 numBytes)
{
    return memcmp(mem1, mem2, (size_t)numBytes);
}

//----------------------------------------------------------------------------------------------------------------------

void memoryClear(void* mem, i64 numBytes)
{
    memset(mem, 0, (size_t)numBytes);
}

//----------------------------------------------------------------------------------------------------------------------

void arenaInit(Arena* arena, i64 initialSize)
{
    u8* buffer = (u8 *)malloc(initialSize);
    if (buffer)
    {
        arena->start = buffer;
        arena->end = arena->start + initialSize;
        arena->cursor = 0;
        arena->restore = -1;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void arenaDone(Arena* arena)
{
    free(arena->start);
    arena->start = 0;
    arena->end = 0;
    arena->cursor = 0;
    arena->restore = -1;
}

//----------------------------------------------------------------------------------------------------------------------

void* arenaAlloc(Arena* arena, i64 size)
{
    void* p = 0;
    if ((arena->start + arena->cursor + size) > arena->end)
    {
        // We don't have enough room
        i64 currentSize = (i64)(arena->end - (u8 *)arena->start);
        i64 requiredSize = currentSize + size;
        i64 newSize = currentSize + MAX(requiredSize, ARENA_INCREMENT);

        u8* newArena = (u8 *)realloc(arena->start, newSize);

        if (newArena)
        {
            arena->start = newArena;
            arena->end = newArena + newSize;

            // Try again!
            p = arenaAlloc(arena, size);
        }
    }
    else
    {
        p = arena->start + arena->cursor;
        arena->cursor += size;
    }

    return p;
}

//----------------------------------------------------------------------------------------------------------------------

void* arenaAlign(Arena* arena)
{
    i64 mod = arena->cursor % ARENA_ALIGN;
    void* p = arena->start + arena->cursor;

    if (mod)
    {
        // We need to align
        arenaAlloc(arena, ARENA_ALIGN - mod);
    }

    return p;
}

//----------------------------------------------------------------------------------------------------------------------

void* arenaAlignedAlloc(Arena* arena, i64 numBytes)
{
    arenaAlign(arena);
    return arenaAlloc(arena, numBytes);
}

//----------------------------------------------------------------------------------------------------------------------

void arenaPush(Arena* arena)
{
    arenaAlign(arena);
    {
        i64* p = arenaAlloc(arena, sizeof(i64) * 2);
        p[0] = 0xaaaaaaaaaaaaaaaa;
        p[1] = arena->restore;
        arena->restore = (i64)((u8 *)p - arena->start);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void arenaPop(Arena* arena)
{
    i64* p = 0;
    ASSERT(arena->restore != -1, "Make sure we have some restore points left");
    arena->cursor = arena->restore;
    p = (i64 *)(arena->start + arena->cursor);
    p[0] = 0xbbbbbbbbbbbbbbbb;
    arena->restore = p[1];
}

//----------------------------------------------------------------------------------------------------------------------

i64 arenaSpace(Arena* arena)
{
    return (arena->end - arena->start) - arena->cursor;
}

//----------------------------------------------------------------------------------------------------------------------

cstr arenaFormatV(Arena* arena, cstr format, va_list args)
{
    i64 c = arena->cursor;
    i64 maxSize = arenaSpace(arena);
    str p = 0;

    int numChars = vsnprintf(arena->start + arena->cursor, maxSize, format, args);
    if (numChars < maxSize)
    {
        // The string fit in the space left.
        p = (char *)arenaAlloc(arena, numChars + 1);
    }
    else
    {
        // There wasn't enough room to hold the string.  Allocate more and try again.
        str p = (char *)arenaAlloc(arena, numChars + 1);
        numChars = vsnprintf(p, numChars + 1, format, args);
    }

    return p;
}

//----------------------------------------------------------------------------------------------------------------------

cstr arenaFormat(Arena* arena, cstr format, ...)
{
    va_list(args);
    va_start(args, format);
    cstr p = arenaFormatV(arena, format, args);
    va_end(args);
    return p;
}

//----------------------------------------------------------------------------------------------------------------------

void* __arrayInternalGrow(void* a, i64 increment, i64 elemSize)
{
    i64 doubleCurrent = a ? 2 * __arrayCapacity(a) : 0;
    i64 minNeeded = arrayCount(a) + increment;
    i64 capacity = doubleCurrent > minNeeded ? doubleCurrent : minNeeded;
    i64 oldBytes = a ? elemSize * arrayCount(a) + sizeof(i64) * 2 : 0;
    i64 bytes = elemSize * capacity + sizeof(i64) * 2;
    i64* p = (i64 *)REALLOC(a ? __arrayRaw(a) : 0, oldBytes, bytes);
    if (p)
    {
        if (!a) p[1] = 0;
        p[0] = capacity;
        return p + 2;
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------{STATE}
//----------------------------------------------------------------------------------------------------------------------
// E M U L A T O R   S T A T E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

typedef struct
{
    Array(u8)       memory;     // The whole memory of the Next
}
State, *StatePtr;

//----------------------------------------------------------------------------------------------------------------------{ENTRY}
//----------------------------------------------------------------------------------------------------------------------
// M A I N   E N T R Y   P O I N T 
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int showCmd)
{
    return 0;
}

