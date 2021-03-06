#include <Windows.h>

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// I N D E X
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
// Search for {KEYWORD}, where KEYWORD is one of the section labels below.
//
// MEMORY       Memory system of the Next.
// STATE        Emulator state.
//
//----------------------------------------------------------------------------------------------------------------------

#include "nx.h"

#include "nextrom.c"
#include "nextmmc.c"

//----------------------------------------------------------------------------------------------------------------------{MEMORY}
//----------------------------------------------------------------------------------------------------------------------
// NEXT MEMORY SYSTEM
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#include "memory.inc"

//----------------------------------------------------------------------------------------------------------------------{STATE}
//----------------------------------------------------------------------------------------------------------------------
// E M U L A T O R   S T A T E
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

typedef struct
{
    Memory      memory;     // The whole memory of the Next
}
State, *StatePtr;

//----------------------------------------------------------------------------------------------------------------------{ENTRY}
//----------------------------------------------------------------------------------------------------------------------
// M A I N   E N T R Y   P O I N T 
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

int kmain(int argc, char** argv)
{
    Memory mem;

    nxMemoryInit(&mem);
    nxMemoryDone(&mem);

    Window mainWnd;
    windowInit(&mainWnd);

    mainWnd.title = stringMake("NX Nova");
    mainWnd.bounds.x = 50;
    mainWnd.bounds.y = 50;
    mainWnd.bounds.w = K_VIEW_SCALE * K_SCREEN_WIDTH;
    mainWnd.bounds.h = K_VIEW_SCALE * K_SCREEN_HEIGHT;
    mainWnd.imageSize.w = K_SCREEN_WIDTH;
    mainWnd.imageSize.h = K_SCREEN_HEIGHT;
    mainWnd.image = K_ALLOC(K_SCREEN_WIDTH * K_SCREEN_HEIGHT * sizeof(u32));
    mainWnd.resizeable = NO;

    windowApply(&mainWnd);

    WindowEvent ev;
    bool quit = NO;
    while (!quit)
    {
        if (windowPoll(&ev))
        {
            switch(ev.type)
            {
            case K_EVENT_QUIT:
                quit = YES;
                break;

            case K_EVENT_KEY:
                if (keyAltPressed(&ev, KEY_1))
                {
                    windowUpdate(&mainWnd);
                    mainWnd.bounds.w = 1 * K_SCREEN_WIDTH;
                    mainWnd.bounds.h = 1 * K_SCREEN_HEIGHT;
                    windowApply(&mainWnd);
                }
                else if (keyAltPressed(&ev, KEY_2))
                {
                    windowUpdate(&mainWnd);
                    mainWnd.bounds.w = 2 * K_SCREEN_WIDTH;
                    mainWnd.bounds.h = 2 * K_SCREEN_HEIGHT;
                    windowApply(&mainWnd);
                }
                else if (keyAltPressed(&ev, KEY_3))
                {
                    windowUpdate(&mainWnd);
                    mainWnd.bounds.w = 4 * K_SCREEN_WIDTH;
                    mainWnd.bounds.h = 4 * K_SCREEN_HEIGHT;
                    windowApply(&mainWnd);
                }

                if (keyPressed(&ev, KEY_ESCAPE))
                {
                    quit = YES;
                }
            }
        }
    }

    windowDone(&mainWnd);
    K_FREE(mainWnd.image, K_SCREEN_WIDTH * K_SCREEN_HEIGHT * sizeof(u32));

    return 0;
}

