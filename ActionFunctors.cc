#include "ActionFunctors.h"
#include "ViWm.h"

namespace Actions
{
    Action::ReturnInfo   VoidAction::operator() ( DesktopLayout &, WindowMakerState & )
        { return Nothing; }

    Action::ReturnInfo SelectUp::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (s.current) {
            s.current.SetTransparency(0);
            s.current = GetNextNode();
            FocusCurrent();
        }
        return Nothing;
    }

    Action::ReturnInfo   SelectDown::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (s.current) {
            s.current->AddTransparency();
            s.current = GetPreviousNode();
            FocusCurrent();
        }
        return Nothing;
    }

    Action::ReturnInfo   MoveToMainArea::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        SwapWindowWithNode(GetFirstNode());
        return NeedRelayout;
    }

    Action::ReturnInfo   ExitAction::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        return Nothing;
    }

    MarginMove::MarginMove( int namount )
        : amount( namount )
    {}

    Action::ReturnInfo   MarginMove::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.margin += amount;
        return NeedRelayout;
    }

    Action::ReturnInfo   IgnoreMode::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.disableNext = !s.disableNext;
        return Nothing;
    }

    Action::ReturnInfo   MouseLock::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (s.lockMouse)
        {
            s.lockMouse = false;
            ClipCursor(0);
        }
        else
        {
            s.lockMouse = true;
            FocusCurrent();
        }
        return Nothing;
    }

    Action::ReturnInfo   SwitchTilling::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.tilingMode = (s.tilingMode + 1) % LastTillingMode;
        return NeedRelayout;
    }

    Action::ReturnInfo   MoveWindowUp::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (current) {
            SwapWindowWithNode(GetNextNode());
            return NeedRelayout;
        }
        return Nothing;
    }

    Action::ReturnInfo   MoveWindowDown::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (s.current) {
            SwapWindowWithNode(GetPreviousNode());
            return NeedRelayout;
        }
        return Nothing;
    }

    Action::ReturnInfo   ShowWindowClass::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        LPSTR temp = (LPSTR)malloc(sizeof(TCHAR) * 128);
        GetClassName(GetForegroundWindow(), temp, 128);
        MessageBox(NULL, temp, "Window Class", MB_OK);
        free(temp);
        return Nothing;
    }

    Action::ReturnInfo   TileWindow::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        if (IsGoodWindow(GetForegroundWindow())) {
            AddNode(GetForegroundWindow());
            return NeedRelayout;
        }
        return Nothing;
    }

    Action::ReturnInfo   UntileWindow::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        RemoveNode(GetForegroundWindow());
        return NeedRelayout;
    }

    SwitchTag::SwitchTag( Tags ntoTag ) : toTag( ntoTag ) {}
    
    Action::ReturnInfo   SwitchTag::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.current_tags = toTag;
        return NeedRelayout;
    }

    MoveTag::MoveTag( Tags ntoTag ) : toTag( ntoTag ) {}
    
    Action::ReturnInfo   MoveTag::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.current->tags = toTag;
        ArrangeWindows();
    }

    MasterAreaChange::MasterAreaChange( int namount )
        : amount( namount ) {}
    
    Action::ReturnInfo   MasterAreaChange::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        s.masterarea_count += amount;
        return NeedRelayout;
    }

    Action::ReturnInfo   CloseForeground::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        PostMessage(GetForegroundWindow(), WM_CLOSE, 0, 0);
        return Nothing;
    }
}
