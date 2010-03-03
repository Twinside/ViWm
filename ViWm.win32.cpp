#include <assert.h>
#include "ViWm.h"
#include "Rendering/RenderWindow.h"

/**
* Ugly as hell, but, should work
*/
extern ViWm::ViWmManager  *globalManager;

namespace ViWm
{
    BOOL ViWmManager::monitorEnumerator( HMONITOR hMonitor
        , HDC    /* hdcMonitor */
        , LPRECT /* intersectionRect */
        , LPARAM userData
        )
    {
        ViWmManager     &instance = *reinterpret_cast<ViWmManager*>( userData );
        DesktopLayout   &myLayout = instance.currentLayout;
        MONITORINFOEX   minfo;

        minfo.cbSize = sizeof( MONITORINFOEX );
        GetMonitorInfo( hMonitor, &minfo );

        int winWidth = abs(minfo.rcWork.right - minfo.rcWork.left);
        int winHeight = abs(minfo.rcWork.bottom - minfo.rcWork.top);

        // yeah our nice window...
        HWND fullScreenWin =
            CreateWindowEx ( WS_EX_LAYERED
                           , "ViWmScreenBack"
                           , "ABack"
                           , WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS 
                            /* style */
                           , minfo.rcWork.left
                           , minfo.rcWork.top
                           , winWidth
                           , winHeight
                           , NULL
                           , NULL /* menu */
                           , GetModuleHandle(0)
                           , NULL
                           );

        if (!fullScreenWin) {
            int err = GetLastError();
            MessageBox(NULL, "Error Creating Background Window", "Error", MB_OK | MB_ICONERROR);
            exit( err ); /* Bail */
        }

        // ok we need our window to be always on bottom, let's hack to get that
        // we also specify our really wanted size =)
        SetWindowPos( fullScreenWin
                    , HWND_BOTTOM
                    , minfo.rcWork.left
                    , minfo.rcWork.top
                    , winWidth
                    , winHeight
                    , /*SWP_NOACTIVATE | */SWP_NOMOVE | SWP_NOSIZE
                    );

        // as MSDN state that some value may be negative for non-primary
        // displays, we abs the width & height to get correct value to
        // work on.
        Renderer::RenderWindow    *newWindow =
            new Renderer::RenderWindow( fullScreenWin
                                      , minfo.rcWork.left
                                      , minfo.rcWork.top
                                      , winWidth
                                      , winHeight );

        Screen          newScreen
            ( *newWindow
            , minfo.rcWork.left
            , minfo.rcWork.top
            , winWidth
            , winHeight
            );

        myLayout.push_back( newScreen );

        // we make the window transparent here
        newWindow->begin();
        newWindow->end();

        assert( myLayout.size() >= 1 );
        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////
    ////                Win32 Event Listeners
    //////////////////////////////////////////////////////////////////////////
    LRESULT ViWmManager::HandleShellHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        if (msg != shellhookid)
            return DefWindowProc(hwnd, msg, wParam, lParam); 

        TilledWindow    *found;

        switch (wParam)
        {
        case HSHELL_WINDOWCREATED:
            if (TilledWindow::IsHandleTileable((HWND)lParam))
            {
                AddNode((HWND)lParam);
                ArrangeWindows();
                FocusCurrent();
                selectWindow( (HWND)lParam );
                _ASSERTE( _CrtCheckMemory() == TRUE );
            }
            break;

        case HSHELL_WINDOWDESTROYED:
            found = currentState.FindNode((HWND)lParam);
            if ( found )
            {
                for (size_t i = 0; i < currentLayout.size(); ++i)
                    LayoutTree::removeClean( currentLayout[i].layoutRoot, (HWND) lParam );

                currentState.RemoveNode((HWND)lParam);

                ArrangeWindows();
                FocusCurrent();
                _ASSERTE( _CrtCheckMemory() == TRUE );
            }
            break;

        case HSHELL_RUDEAPPACTIVATED:
        case HSHELL_WINDOWACTIVATED:
            found = currentState.FindNode((HWND)lParam);
            if (found) {
                if (currentState.current) {
                    currentState.current->SetTransparency( currentState.alpha );
                }
                currentState.current = found;
                found->SetTransparency( 255 );
                selectWindow( (HWND)lParam );

                FocusCurrent();
            }
            _ASSERTE( _CrtCheckMemory() == TRUE );
            break;
        }
        return 0;
    }

    LRESULT CALLBACK ViWmManager::keyListenerProc( HWND hwnd, UINT msg
                                                 , WPARAM wParam, LPARAM lParam)
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
            PostQuitMessage(WM_QUIT);
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

    LRESULT CALLBACK ViWmManager::windowListenerProc( HWND hwnd , UINT msg 
                                                    , WPARAM wParam , LPARAM lParam )
    {
        switch (msg)
        {
        case WM_NCCREATE: return TRUE;
        case WM_CREATE: return 0;

        // we handle this message to be able
        // to get back mouse tracking messages.
        case WM_NCHITTEST: return HTCLIENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_CLOSE: break;

        case WM_NCLBUTTONDOWN:
            // get selected band
            break;

        case WM_MOUSEMOVE:
            // move the splits
            break;

        default:
            DefWindowProc(hwnd, msg, wParam, lParam); 
        }
        return 0;
    }
}
