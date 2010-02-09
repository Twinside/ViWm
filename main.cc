#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "ViWm.h"

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
    MSG msg;

    globalManager = new ViWm( hInstance );
    globalManager->Init();

    EnumWindows(EnumWindowsProc, 0);

    //ArrangeWindows();

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
