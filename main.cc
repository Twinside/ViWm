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
#include "resource.h"

using namespace ViWm;

ViWmManager    *globalManager;

const LPTSTR fullScreenWindowClassName = TEXT("ViWmScreenBack");
const LPTSTR keyListenerClassName = TEXT("ViWm");

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
                  , TEXT("The programm cannot start because we cannot start the COM interface")
                  , TEXT("ViWm - Launching Error")
                  , MB_OK );
    }

    defs.push_back( HotKey( 'V'
                          , new Actions::VerticalSplit( IDS_ACTION_SPLIT_VERTICALY, NoStringId ) ));
    defs.push_back( HotKey( 'S'
                          , new Actions::HorizontalSplit( IDS_ACTION_SPLIT_HORIZONTALLY, NoStringId ) ));
    defs.push_back( HotKey( 'R'
                          , new Actions::NodeRotate( IDS_ACTION_ROTATE, NoStringId ,1 ) ));
    defs.push_back( HotKey( 'I'
                          , new Actions::InverseScreenLayout( IDS_ACTION_INVERSELAYOUT, NoStringId )));
    defs.push_back( HotKey( 'L'
                          , new Actions::Relayout( IDS_ACTION_RELAYOUT, NoStringId )));
    defs.push_back( HotKey(  VK_LEFT
                          , new Actions::ScreenMoverBounded( IDS_ACTION_MOVEPREVSCREEN, NoStringId ,-1 )));
    defs.push_back( HotKey( VK_RIGHT
                          , new Actions::ScreenMoverBounded( IDS_ACTION_MOVENEXTSCREEN, NoStringId, 1 )));

    globalManager = new ViWmManager( hInstance, defs );
    globalManager->Init();

    globalManager->AddHotkey(
        HotKey( 'Q', new Actions::ExitAction( IDS_ACTION_QUIT
                                            , NoStringId
                                            ,globalManager->GetReceivingWindow()) ));

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
