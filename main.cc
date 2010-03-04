#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "ViWm.h"
#include "Actions/TreeAction.h"
#include "Actions/MultiScreen.h"
#include "Actions/System.h"

using namespace ViWm;

ViWmManager    *globalManager;

const char* fullScreenWindowClassName = "ViWmScreenBack";
const char* keyListenerClassName = "ViWm";

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    reinterpret_cast<ViWmManager*>( lParam )->AddNode( hwnd );
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

    HRESULT hr = CoInitialize( NULL );

    if (!SUCCEEDED(hr))
    {
        MessageBox( NULL
                   , "The programm cannot start because we cannot start the COM interface"
                   , "ViWm - Launching Error", MB_OK );
    }

    defs.push_back( HotKey( 'V', new Actions::VerticalSplit() ));
    defs.push_back( HotKey( 'S', new Actions::HorizontalSplit() ));
    defs.push_back( HotKey( 'R', new Actions::NodeRotate( 1 ) ));
    defs.push_back( HotKey(  VK_LEFT, new Actions::ScreenMoverBounded( -1 )));
    defs.push_back( HotKey( VK_RIGHT, new Actions::ScreenMoverBounded( 1 )));

    globalManager = new ViWmManager( hInstance, defs );
    globalManager->Init();

    globalManager->AddHotkey(
        HotKey( 'Q', new Actions::ExitAction(globalManager->GetReceivingWindow()) ));

    EnumWindows(EnumWindowsProc, (LPARAM)globalManager);

    //ArrangeWindows();

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();

    return msg.wParam;
}
