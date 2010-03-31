#include "WindowCapture.h"

namespace ViWm {
namespace Actions
{
    WindowCapture::WindowCapture( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    Action::ReturnInfo   WindowCapture::operator() ( DesktopLayout&    layout
                                                   , WindowMakerState& state )
    {
        HWND focused = GetForegroundWindow();

        if ( focused == NULL )
            return Nothing;

        TilledWindow    *newWindow = new TilledWindow( focused );
        state.addWindow( *newWindow );

        state.getCurrentLayouter().addNewWindowToLayout
                ( *newWindow, state, layout );

        return NeedRelayout;
    }

    WindowRelease::WindowRelease( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    Action::ReturnInfo   WindowRelease::operator() ( DesktopLayout&    layout
                                                   , WindowMakerState& state )
    {
        HWND focused = GetForegroundWindow();

        TilledWindow *found = state.FindNode(focused);

        if ( !found )
            return Nothing;

        for (size_t i = 0; i < layout.size(); ++i)
            LayoutTree::removeClean( layout[i].layoutRoot, focused );

        state.RemoveNode(focused);

        return NeedRelayout;
    }
}}


