#include "ActionFunctors.h"
#include "ViWm.h"

namespace Actions
{
    Action::ReturnInfo   VoidAction::operator() ( DesktopLayout &, WindowMakerState & )
        { return Nothing; }

    Action::ReturnInfo SelectUp::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        if (s.current) {
            s.current->SetTransparency(0);
            s.current = GetNextNode();
            FocusCurrent();
        }
        #endif
        return Nothing;
    }

    Action::ReturnInfo   SelectDown::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
#if 0
        if (s.current) {
            s.current->AddTransparency();
            s.current = GetPreviousNode();
            FocusCurrent();
        }
#endif
        return Nothing;
    }

    Action::ReturnInfo   MoveToMainArea::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
#if 0
        SwapWindowWithNode(GetFirstNode());
#endif
        return NeedRelayout;
    }

    Action::ReturnInfo   ExitAction::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
#if 0
        PostMessage(hwnd, WM_CLOSE, 0, 0);
#endif
        return Nothing;
    }

    MarginMove::MarginMove( int namount )
        : amount( namount )
    {}

    Action::ReturnInfo   MarginMove::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.margin += amount;
        #endif
        return NeedRelayout;
    }

    Action::ReturnInfo   IgnoreMode::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.disableNext = !s.disableNext;
        #endif
        return Nothing;
    }

    Action::ReturnInfo   MouseLock::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
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
        
        #endif
        return Nothing;
    }

    Action::ReturnInfo   SwitchTilling::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.tilingMode = static_cast<TillingMode>((s.tilingMode + 1) % LastTillingMode);
        
        #endif
        return NeedRelayout;
    }

    Action::ReturnInfo   MoveWindowUp::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        if (s.current) {
            SwapWindowWithNode(GetNextNode());
            return NeedRelayout;
        }
        
        #endif
        return Nothing;
    }

    Action::ReturnInfo   MoveWindowDown::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        if (s.current) {
            SwapWindowWithNode(GetPreviousNode());
            return NeedRelayout;
        }
        
        #endif
        return Nothing;
    }

    Action::ReturnInfo   ShowWindowClass::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        LPSTR temp = (LPSTR)malloc(sizeof(TCHAR) * 128);
        GetClassName(GetForegroundWindow(), temp, 128);
        MessageBox(NULL, temp, "Window Class", MB_OK);
        free(temp);
        #endif
        return Nothing;
    }

    Action::ReturnInfo   TileWindow::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        if (IsGoodWindow(GetForegroundWindow())) {
            AddNode(GetForegroundWindow());
            return NeedRelayout;
        }
        
        #endif
        return Nothing;
    }

    Action::ReturnInfo   UntileWindow::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        RemoveNode(GetForegroundWindow());
        #endif

        return NeedRelayout;
    }

    SwitchTag::SwitchTag( Tags ntoTag ) : toTag( ntoTag ) {}
    
    Action::ReturnInfo   SwitchTag::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.currentTag = toTag;
        #endif
        return NeedRelayout;
    }

    MoveTag::MoveTag( Tags ntoTag ) : toTag( ntoTag ) {}
    
    Action::ReturnInfo   MoveTag::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.current->tags = toTag;
        #endif
        return NeedRelayout;
    }

    MasterAreaChange::MasterAreaChange( int namount )
        : amount( namount ) {}
    
    Action::ReturnInfo   MasterAreaChange::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        #if 0
        s.masterarea_count += amount;
        #endif
        return NeedRelayout;
    }

    Action::ReturnInfo   CloseForeground::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        PostMessage(GetForegroundWindow(), WM_CLOSE, 0, 0);
        return Nothing;
    }
}
