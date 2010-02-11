#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "ViWm.h"
#include "Actions/TreeAction.h"

ViWm    *globalManager;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        return 0;
        break;

    case WM_CLOSE:
        ClipCursor(0); 	/* Release Cursor Lock */
        delete globalManager;
        globalManager = 0;
        break;

    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;

    case WM_HOTKEY:
        globalManager->HandleHotKey( wParam );
        break;

    default:
        return globalManager->HandleShellHook( hwnd, msg, wParam, lParam );
    }
    return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    reinterpret_cast<ViWm*>( lParam )->AddNode( hwnd );
    return TRUE;
}

int WINAPI WinMain( HINSTANCE hInstance
                  , HINSTANCE /*hPrevInstance*/
                  , LPSTR /*lpCmdLine*/
                  , int /*nShowCmd*/ )
{
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF
                  | _CRTDBG_CHECK_ALWAYS_DF
                  | _CRTDBG_DELAY_FREE_MEM_DF
                  );
#endif

    MSG msg;
    HotKeyCollection    defs;

    defs.push_back( HotKey( 'V', new Actions::VerticalSplit() ));
    defs.push_back( HotKey( 'S', new Actions::HorizontalSplit() ));
    defs.push_back( HotKey( 'R', new Actions::NodeRotate( 1 ) ));

    globalManager = new ViWm( hInstance, defs );
    globalManager->Init();

    EnumWindows(EnumWindowsProc, (LPARAM)globalManager);

    //ArrangeWindows();

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
