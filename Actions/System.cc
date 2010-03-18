#include "System.h"

namespace ViWm {
namespace Actions
{
    ExitAction::ExitAction( StringId display, StringId descr
                          , LayoutTree::WindowKey key )
        : Action( display, descr )
        , toSend( key )
    {}

    Action::ReturnInfo   ExitAction::operator() ( DesktopLayout& /*l*/
                                                , WindowMakerState& /*s*/ )
    {
#ifdef WIN32
        PostMessage(toSend, WM_CLOSE, 0, 0);
#endif /* WIN32 */
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

    InverseScreenLayout::InverseScreenLayout( StringId display, StringId descr )
        : Action( display, descr )
    {}
}}
