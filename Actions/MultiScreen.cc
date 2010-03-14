#include <algorithm>
#include "MultiScreen.h"

#undef min
#undef max

namespace ViWm {
namespace Actions
{
    ScreenMoverBounded::ScreenMoverBounded( StringId display
                                          , StringId descr
                                          , int amount)
        : Action( display, descr )
        , _moveAmount( amount )
    {}

    Action::ReturnInfo ScreenMoverBounded::operator()( DesktopLayout &l
                                                     , WindowMakerState &state )
    {
        LayoutTree  *currentTree =
            l[ state.currentScreen ].layoutRoot;

        size_t nextScreen =
            std::max<int>( std::min<int>( int(state.currentScreen) + _moveAmount
                                        , int(l.size()) - 1)
                         , 0 );

        if (!currentTree || state.currentScreen == nextScreen)
            return Nothing;

        LayoutLeaf* selected =
            static_cast<LayoutLeaf*>(currentTree->getSelected());

        LayoutTree::addCreate( l[ nextScreen ].layoutRoot
                             , *new LayoutLeaf( selected->getWindow()) );

        l[ nextScreen ].layoutRoot->selectNode(
                selected->getWindow().getWinowKey());

        LayoutTree::removeClean( l[ state.currentScreen ].layoutRoot
                               , selected );

        // force update of the old screen
        state.getCurrentLayouter().layout( state, l );
        l[ state.currentScreen ].replace( true );

        state.currentScreen = nextScreen;

        return NeedRelayout;
    }
}}
