#include "ViWm.h"
#include "Action.h"
#include "Layouter.h"
#include "Layouter/Layouts.h"
#include "LayoutTree.h"

namespace ViWm
{
    ViWmManager::ViWmManager( HINSTANCE inithInstance
        , const HotKeyCollection &originalCollection
        )
        : modkeys( DEFAULT_MODKEY )
        , hInstance( inithInstance )
        , shellhookid( (UINT)-1 )
        , hotkeysDefinition( originalCollection )
    {
        layouter.reserve( LastTillingMode );
        layouter.push_back( LayoutPtr( new Layout::VerticalLayout() ) );
        layouter.push_back( LayoutPtr( new Layout::HorizontalLayout() ) );
        layouter.push_back( LayoutPtr( new Layout::FullScreenLayout() ) );
        layouter.push_back( LayoutPtr( new Layout::ManualVimLayout() ) );
    }

    ViWmManager::~ViWmManager()
    {
        DeregisterShellHookWindow( globalHotkeyListener );
        UnregisterHotKeys( globalHotkeyListener );
    }

    void ViWmManager::Init()
    {
        createGlobalListener( hInstance );

        EnumDisplayMonitors( NULL, NULL, &ViWmManager::monitorEnumerator
                           , reinterpret_cast<LPARAM>(this) );
    }

    void ViWmManager::HandleHotKey( WPARAM wParam )
    {
        size_t  hotkeyIndex = static_cast<size_t>( wParam );

        // if we are out of bound, we ignore the message.
        if ( hotkeyIndex >= hotkeysDefinition.size() )
            return;

        switch ((*hotkeysDefinition[hotkeyIndex].second)( currentLayout, currentState ))
        {
        case Action::Nothing: break;

        case Action::NeedRelayout:
            ArrangeWindows();
            break;
        }
    }

    void ViWmManager::UnregisterHotKeys(HWND hwnd)
    {
        for (size_t i = 0; i < hotkeysDefinition.size(); i++)
            UnregisterHotKey(hwnd, i);
    }

    void ViWmManager::AddHotkey( HotKey k )
    {
        RegisterHotKey( globalHotkeyListener
                      , hotkeysDefinition.size()
                      , modkeys
                      , k.first );

        hotkeysDefinition.push_back( k );
    }

    void ViWmManager::RegisterHotKeys(HWND hwnd)
    {
        HotKeyCollection::const_iterator    it;
        int i = 0;

        for ( it = hotkeysDefinition.begin()
            ; it != hotkeysDefinition.end()
            ; ++it )
        {
            RegisterHotKey(hwnd, i++, modkeys, it->first );
        }
    }

    void ViWmManager::createGlobalListener( HINSTANCE hInstance )
    {
        WNDCLASSEX winClass;

        winClass.cbSize = sizeof(WNDCLASSEX);
        winClass.style = 0;
        winClass.lpfnWndProc = keyListenerProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = hInstance;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = keyListenerClassName;

        if (!RegisterClassEx(&winClass)) {
            MessageBox(NULL, "Error Registering Window Class", "Error", MB_OK | MB_ICONERROR);
            exit(1); /* Bail */
        }

        // create a message only window
        globalHotkeyListener = CreateWindowEx( 0, keyListenerClassName, "ViWm"
            , 0, 0, 0, 0, 0
            , HWND_MESSAGE, NULL
            , hInstance, NULL);

        if (!globalHotkeyListener) {
            int err = GetLastError();
            MessageBox(NULL, "Error Creating Window", "Error", MB_OK | MB_ICONERROR);
            exit( err ); /* Bail */
        }

        // class for full screen windows
        winClass.cbSize = sizeof(WNDCLASSEX);
        winClass.style = CS_HREDRAW | CS_VREDRAW;
        winClass.lpfnWndProc = windowListenerProc;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = hInstance;
        winClass.hIcon = NULL;
        winClass.hIconSm = NULL;
        winClass.hCursor = NULL;
        winClass.hbrBackground = NULL;
        winClass.lpszMenuName = NULL;
        winClass.lpszClassName = fullScreenWindowClassName;

        if (!RegisterClassEx(&winClass)) {
            MessageBox( NULL
                      , "Error While creating Background Window Class"
                      , "Error"
                      , MB_OK | MB_ICONERROR);

            exit(1); /* Bail */
        }
        RegisterHotKeys( globalHotkeyListener );

        RegisterShellHookWindow( globalHotkeyListener );
        shellhookid = RegisterWindowMessage("SHELLHOOK");
    }

    void ViWmManager::ArrangeWindows()
    {
        layouter[currentState.tilingMode]->layout( currentState, currentLayout );
        if ( currentLayout.size() > currentState.currentScreen )
        {
            currentLayout[currentState.currentScreen].replace( true );
        }
        FocusCurrent();
    }


    void ViWmManager::AddNode( HWND hwnd )
    {
        OutputDebugString( __FUNCTION__ "\n" );

        char TempClassName[MAX_PATH];
        GetClassName( hwnd, TempClassName, MAX_PATH );


#ifdef _DEBUG
        // For debug purpose and avoiding to messing up with
        // the whole desktop, we only handle the notepad program
        // which has the good taste to be lightweight.
        if ( strcmp( TempClassName, "Notepad") != 0 )
            return;
#endif

        TilledWindow    *newWindow = new TilledWindow( hwnd );
        currentState.windowList[ currentState.currentTag ]
        .windowList
            .push_back( newWindow );

        layouter[currentState.tilingMode]->addNewWindowToLayout( *newWindow
            , currentState
            , currentLayout );
        ArrangeWindows();

    }

    void ViWmManager::FocusCurrent()
    {
        if ( currentState.current )
        {
            currentState.current->GiveFocus();
        }
    }

    void ViWmManager::selectWindow( HWND currentWindow )
    {
        if ( currentLayout.size() <= currentState.currentScreen )
            return;

        Screen  *scr = &currentLayout[ currentState.currentScreen ];
        if ( scr->layoutRoot
            && scr->layoutRoot->selectNode( currentWindow ) != LayoutTree::Searching)
            return;

        for ( size_t i = 0; i < currentLayout.size(); i++ )
        {
            if ( i == currentState.currentScreen ) continue;
            LayoutTree  *root = currentLayout[i].layoutRoot;
            if ( root && root->selectNode( currentWindow ) != LayoutTree::Searching )
            {
                currentState.currentScreen = i;
                return;
            }
        }
    }

    void    ViWmManager::beginPick( int x, int y )
    {
        for ( size_t i = 0; i < currentLayout.size(); i++ )
        {
            if ( currentLayout[i].isInScreenBound( x, y ) )
            {
                currentSplitMove = currentLayout[i].FindPointedSplit( x, y );
                currentScreenMove = i;
                currentLayout[i].replace( false );
                currentLayout[i].putAtTop();
            }
        }
    }

    LayoutTree::SplitSide ViWmManager::QuerySplitDirection( int x, int y )
    {
        // if now currently dragging
        if ( currentSplitMove.first != NULL )
            return currentSplitMove.first->getLastDirection();

        // we find the good split =)
        for ( size_t i = 0; i < currentLayout.size(); i++ )
        {
            if ( currentLayout[i].isInScreenBound( x, y ) )
            {
                LayoutNode *n = currentLayout[i].FindPointedSplit( x, y ).first;
                return n ? n->getLastDirection() : LayoutTree::SplitHorizontal;
            }
        }
        return LayoutTree::SplitHorizontal;
    }

    void    ViWmManager::movePick( int x, int y )
    {
        if ( currentSplitMove.first == NULL )
            return;

        Screen &s = currentLayout[ currentScreenMove ];
        currentSplitMove.first->moveSplit( s , x - s.getX(), y - s.getY()
                                         , currentSplitMove.second );

        s.replace( false );
    }

    void    ViWmManager::endPick()
    {
        currentSplitMove = LayoutTree::SplitCoord( NULL, 0 );
        currentLayout[currentScreenMove].replace( true );
        currentLayout[currentScreenMove].putAtBottom();
    }
}
