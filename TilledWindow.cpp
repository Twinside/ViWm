#include "TilledWindow.h"

namespace ViWm
{
#ifdef REALCODE
    TilledWindow::TilledWindow(HWND realWindow)
        : tags( 0 )
        , hwnd( realWindow )
        , applyedAlpha( 0 )
        , hasBorder( true )
    {
        SetBorders( false );
        SetTransparency( Opaque );
        GetWindowRect( realWindow, &initialSize );
    }

    TilledWindow::~TilledWindow()
    {
        if ( hwnd )
        {
            SetTransparency( Opaque );
            SetBorders( true );
        	ShowWindow(hwnd, SW_RESTORE);
            SetWindowPos( hwnd, NULL
                        , initialSize.left
                        , initialSize.top
                        , initialSize.right - initialSize.left
                        , initialSize.bottom - initialSize.top
                        , SWP_SHOWWINDOW
                        );
        }
    }

    TCHAR   TilledWindow::TempClassName[MAX_PATH];

    void TilledWindow::SetTransparency(int alpha)
    {
        if ( alpha != applyedAlpha )
        {
            if ( alpha < 255 )
            {
                SetWindowLong( hwnd
                             , GWL_EXSTYLE
                             , GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED
                             );

                SetLayeredWindowAttributes( hwnd
                                          , RGB(0, 0, 0)
                                          , static_cast<BYTE>(alpha)
                                          , LWA_ALPHA
                                          );
            }
            else
            {
                SetWindowLong( hwnd
                             , GWL_EXSTYLE
                             , GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED
                             );
            }
        }
        applyedAlpha = alpha;
    }

    void TilledWindow::SetBorders( bool presentOrNot )
    {
        if ( presentOrNot == hasBorder ) return;

        if (presentOrNot)
        {
            SetWindowLong(hwnd, GWL_STYLE, (GetWindowLong(hwnd, GWL_STYLE) | (WS_CAPTION | WS_SIZEBOX)));
            SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); 	/* Fix? */
        }
        else
        {
            SetWindowLong(hwnd, GWL_STYLE, (GetWindowLong(hwnd, GWL_STYLE) & ~(WS_CAPTION | WS_SIZEBOX)));
        }
        hasBorder = presentOrNot;
    }

    bool TilledWindow::IsHandleTileable( HWND hwnd )
    {
        /* Some criteria for windows to be tiled */
        if ( IsWindowVisible(hwnd) 
          && GetParent(hwnd) == 0 ) // no parent <=> Top Level window ?
        {
            int exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            HWND owner = GetWindow(hwnd, GW_OWNER);

            return ((exstyle & WS_EX_TOOLWINDOW) == 0 && owner == 0)
                || ((exstyle & WS_EX_APPWINDOW)       && owner != 0);
        }
        return false;
    }

    bool TilledWindow::IsTileable() { return IsHandleTileable( hwnd ); }

    void TilledWindow::GiveFocus()
    {
        SetForegroundWindow(hwnd);
        SetTransparency( Opaque );
    }

    void TilledWindow::SetSize( int x, int y, int width, int height )
    {
        // Why HWND_BOTTOM ?
        // well for a weird reason it appeared that the first window
        // wasn't cool enough to stay where it was, and was at front.
        // This was really annoying when a split is moved, and trying
        // to reduce the first window of the layout level.
        ::SetWindowPos( hwnd, HWND_BOTTOM, x, y, width, height, NULL );
    }

    void TilledWindow::GetClassName( std::wstring &name )
    {
        ::GetClassName( hwnd, TempClassName, MAX_PATH );
        name = TempClassName;
    }
#else   /* TEST */
    TilledWindow::TilledWindow(HWND realWindow)
        : tags( 0 )
        , hwnd( realWindow )
        , applyedAlpha( 0 )
        , hasBorder( true )
    {}

    TilledWindow::~TilledWindow() {}

    TCHAR   TilledWindow::TempClassName[MAX_PATH];

    void TilledWindow::SetTransparency(int /*alpha*/) {}
    void TilledWindow::SetBorders( bool /*presentOrNot*/ ) {}
    bool TilledWindow::IsHandleTileable( HWND /*hwnd*/ )
        { return true; }
    bool TilledWindow::IsTileable() { return IsHandleTileable( hwnd ); }
    void TilledWindow::GiveFocus() {}
    void TilledWindow::SetSize( int /*x*/, int /*y*/
                              , int /*width*/, int /*height*/ ) {}
    void TilledWindow::GetClassName( std::wstring &name )
        { name = L"meh"; }
#endif  /* TEST */
}

