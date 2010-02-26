#include "ViWm.h"


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

        // yeah our nice window...
        HWND fullScreenWin =
            CreateWindowEx ( WS_EX_LAYERED
                            | WS_EX_LEFT
                            | WS_EX_LTRREADING

                            | WS_EX_APPWINDOW
                            | WS_EX_TOOLWINDOW
                            | WS_EX_NOACTIVATE

                           , "ViWmScreenBack"
                           , "ABack"
                           , WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS
                            /* style */

                           // fake size as it will be changed later
                           , minfo.rcWork.left
                           , minfo.rcWork.top
                           , abs(minfo.rcWork.right - minfo.rcWork.left)
                           , abs(minfo.rcWork.bottom - minfo.rcWork.top)
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
                    , abs(minfo.rcWork.right - minfo.rcWork.left)
                    , abs(minfo.rcWork.bottom - minfo.rcWork.top)
                    , 0/*SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE */
                    );

        ShowWindow( fullScreenWin, SW_SHOW);
        // Ask the window and its children to repaint
        RedrawWindow(fullScreenWin, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME |
            RDW_ALLCHILDREN);

        // as MSDN state that some value may be negative for non-primary
        // displays, we abs the width & height to get correct value to
        // work on.
        Screen          newScreen
            ( instance.defaultRenderer.CreateRenderWindow( fullScreenWin )
            , minfo.rcWork.left
            , minfo.rcWork.top
            , abs(minfo.rcWork.right - minfo.rcWork.left)
            , abs(minfo.rcWork.bottom - minfo.rcWork.top)
            );

        myLayout.push_back( newScreen );

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
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rc;
        POINT aptStar[6] = {50,2, 2,98, 98,33, 2,33, 98,98, 50,2}; 

        switch (msg)
        {
        case WM_NCCREATE: return TRUE;
        case WM_CREATE: return 0;

        case WM_CLOSE: break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps); 
            GetClientRect(hwnd, &rc); 
            SetMapMode(hdc, MM_ANISOTROPIC); 
            SetWindowExtEx(hdc, 100, 100, NULL); 
            SetViewportExtEx(hdc, rc.right, rc.bottom, NULL); 
            Polyline(hdc, aptStar, 6); 
            EndPaint(hwnd, &ps); 
            break;

        default:
            DefWindowProc(hwnd, msg, wParam, lParam); 
        }
        return 0;
    }
}
