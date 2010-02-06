#include "TilledWindow.h"

TilledWindow::TilledWindow(HWND realWindow)
    : applyedAlpha( 0 )
    , hwnd( realWindow )
    , hasBorder( true )
{
    SetBorders( false );
    SetTransparency( Opaque );
}

TilledWindow::~TilledWindow()
{
    SetTransparency( Opaque );
    SetBorders( true );
	ShowWindow(hwnd, SW_RESTORE);
}

void TilledWindow::SetTransparency(int alpha)
{
    if ( alpha != applyedAlpha )
    {
        if ( alpha >= 255 )
        {
            SetWindowLong( hwnd
                         , GWL_EXSTYLE
                         , GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED
                         );

            SetLayeredWindowAttributes( hwnd
                                      , RGB(255, 0, 0)
                                      , static_cast<BYTE>(alpha)
                                      , LWA_COLORKEY | LWA_ALPHA
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

bool TilledWindow::IsTileable()
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