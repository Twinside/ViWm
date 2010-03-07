#include "System.h"

namespace ViWm {
namespace Actions
{
    ExitAction::ExitAction( LayoutTree::WindowKey key )
        : toSend( key )
    {}

    Action::ReturnInfo   ExitAction::operator() ( DesktopLayout &l, WindowMakerState &s )
    {
        PostMessage(toSend, WM_CLOSE, 0, 0);
        return Nothing;
    }

    Action::ReturnInfo InverseScreenLayout::operator()( DesktopLayout &l, WindowMakerState &s )
    {
        Screen &screen = l[s.currentScreen];

        if ( screen.initialSplit == LayoutTree::SplitVertical )
            screen.initialSplit = LayoutTree::SplitHorizontal;
        else
            screen.initialSplit = LayoutTree::SplitVertical;

        return NeedRelayout;
    }
}}
