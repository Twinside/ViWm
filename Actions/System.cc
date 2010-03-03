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
}}
