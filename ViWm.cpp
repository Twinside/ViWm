#include "ViWm.h"
#include "Action.h"
#include "Layouter.h"
#include "Layouter/Layouts.h"
#include "LayoutTree.h"

ViWm::ViWm( HINSTANCE inithInstance
          , const HotKeyCollection &originalCollection
          )
    : modkeys( DEFAULT_MODKEY )
    , hInstance( inithInstance )
    , shellhookid( (UINT)-1 )
    , hotkeysDefinition( originalCollection )
{
    EnumDisplayMonitors( NULL, NULL, &ViWm::monitorEnumerator
                       , reinterpret_cast<LPARAM>(&currentLayout) );

    layouter.reserve( LastTillingMode );
    layouter.push_back( LayoutPtr( new Layout::VerticalLayout() ) );
    layouter.push_back( LayoutPtr( new Layout::HorizontalLayout() ) );
    layouter.push_back( LayoutPtr( new Layout::FullScreenLayout() ) );
    layouter.push_back( LayoutPtr( new Layout::ManualVimLayout() ) );
}

ViWm::~ViWm()
{
    DeregisterShellHookWindow( globalHotkeyListener );
    UnregisterHotKeys( globalHotkeyListener );
}

void ViWm::Init()
{
    createGlobalListener( hInstance );
}

void ViWm::HandleHotKey( WPARAM wParam )
{
    size_t  hotkeyIndex = static_cast<size_t>( wParam ) - 1;

    // if we are out of bound, we ignore the message.
    if ( hotkeyIndex >= hotkeysDefinition.size() )
        return;

    switch ((*hotkeysDefinition[hotkeyIndex - 1].second)( currentLayout, currentState ))
    {
    case Action::Nothing: break;

    case Action::NeedRelayout:
        ArrangeWindows();
        break;
    }
}

LRESULT ViWm::HandleShellHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (msg != shellhookid)
        return DefWindowProc(hwnd, msg, wParam, lParam); 

    TilledWindow    *found;

    switch (wParam)
    {
    case HSHELL_WINDOWCREATED:
        if (TilledWindow::IsHandleTileable((HWND)lParam))
        {
            _ASSERTE( _CrtCheckMemory() == TRUE );
            AddNode((HWND)lParam);
            _ASSERTE( _CrtCheckMemory() == TRUE );
            ArrangeWindows();
            _ASSERTE( _CrtCheckMemory() == TRUE );
            FocusCurrent();
            _ASSERTE( _CrtCheckMemory() == TRUE );
        }
        break;

    case HSHELL_WINDOWDESTROYED:
        found = currentState.FindNode((HWND)lParam);
        if ( found )
        {
            _ASSERTE( _CrtCheckMemory() == TRUE );
            for (size_t i = 0; i < currentLayout.size(); ++i)
                LayoutTree::removeClean( currentLayout[i].layoutRoot, (HWND) lParam );

            _ASSERTE( _CrtCheckMemory() == TRUE );
            currentState.RemoveNode((HWND)lParam);
            _ASSERTE( _CrtCheckMemory() == TRUE );

            ArrangeWindows();
            _ASSERTE( _CrtCheckMemory() == TRUE );
            FocusCurrent();
            _ASSERTE( _CrtCheckMemory() == TRUE );
        }
        break;

    case HSHELL_WINDOWACTIVATED:
        found = currentState.FindNode((HWND)lParam);
        _ASSERTE( _CrtCheckMemory() == TRUE );
        if (found) {
            if (currentState.current) {
                currentState.current->SetTransparency( currentState.alpha );
            }
            currentState.current = found;
            _ASSERTE( _CrtCheckMemory() == TRUE );
            FocusCurrent();
            _ASSERTE( _CrtCheckMemory() == TRUE );
        }
        break;
    }
    return 0;
}

void ViWm::UnregisterHotKeys(HWND hwnd)
{
    for (size_t i = 1; i <= hotkeysDefinition.size(); i++)
        UnregisterHotKey(hwnd, i);
}

void ViWm::RegisterHotKeys(HWND hwnd)
{
    HotKeyCollection::const_iterator    it;
    int i = 1;

    for ( it = hotkeysDefinition.begin()
        ; it != hotkeysDefinition.end()
        ; ++it, i++ )
    {
        RegisterHotKey(hwnd, i, modkeys, it->first );
    }
}

/**
 * Ugly as hell, but, should work
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ViWm::createGlobalListener( HINSTANCE hInstance )
{
    WNDCLASSEX winClass;

    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = 0;
    winClass.lpfnWndProc = WndProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = hInstance;
    winClass.hIcon = NULL;
    winClass.hIconSm = NULL;
    winClass.hCursor = NULL;
    winClass.hbrBackground = NULL;
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = "ViWm";

    if (!RegisterClassEx(&winClass)) {
        MessageBox(NULL, "Error Registering Window Class", "Error", MB_OK | MB_ICONERROR);
        exit(1); /* Bail */
    }

    // create a message only window
    globalHotkeyListener = CreateWindowEx( 0, "ViWm", "ViWm"
                                         , 0, 0, 0, 0, 0
                                         , HWND_MESSAGE, NULL
                                         , hInstance, NULL);

    if (!globalHotkeyListener) {
        int err = GetLastError();
        MessageBox(NULL, "Error Creating Window", "Error", MB_OK | MB_ICONERROR);
        exit( err ); /* Bail */
    }

    RegisterHotKeys( globalHotkeyListener );

    RegisterShellHookWindow( globalHotkeyListener );
    shellhookid = RegisterWindowMessage("SHELLHOOK");
}

void ViWm::ArrangeWindows()
{
    layouter[currentState.tilingMode]->layout( currentState, currentLayout );
    currentLayout[currentState.currentScreen].replace();
    FocusCurrent();
}


BOOL ViWm::monitorEnumerator( HMONITOR hMonitor
                            , HDC    /* hdcMonitor */
                            , LPRECT /* intersectionRect */
                            , LPARAM userData
                            )
{
    DesktopLayout   &myLayout = *reinterpret_cast<DesktopLayout*>( userData );
    MONITORINFOEX   minfo;

    minfo.cbSize = sizeof( MONITORINFOEX );
    GetMonitorInfo( hMonitor, &minfo );

    // as MSDN state that some value may be negative for non-primary
    // displays, we abs the width & height to get correct value to
    // work on.
    Screen          newScreen( minfo.rcWork.left
                             , minfo.rcWork.top
                             , abs(minfo.rcWork.right - minfo.rcWork.left)
                             , abs(minfo.rcWork.bottom - minfo.rcWork.top)
                             );

    myLayout.push_back( newScreen );

    return TRUE;
}

void ViWm::AddNode( HWND hwnd )
{
    OutputDebugString( __FUNCTION__ "\n" );

    char TempClassName[MAX_PATH];
    GetClassName( hwnd, TempClassName, MAX_PATH );

    // DEBUG!!
    if ( strcmp( TempClassName, "Notepad") != 0 )
        return;

    TilledWindow    *newWindow = new TilledWindow( hwnd );
    currentState.windowList[ currentState.currentTag ]
                           .windowList
                           .push_back( newWindow );

    layouter[currentState.tilingMode]->addNewWindowToLayout( *newWindow
                                                           , currentState
                                                           , currentLayout );
    ArrangeWindows();

}

void ViWm::FocusCurrent()
{
}
