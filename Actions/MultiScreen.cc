#include <algorithm>
#include "MultiScreen.h"

#undef min
#undef max

namespace ViWm {
namespace Actions
{
    ScreenMoverBounded::ScreenMoverBounded(int amount)
        : _moveAmount( amount )
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
                             , *new LayoutLeaf( selected->window ) );

        l[ nextScreen ].layoutRoot->selectNode( selected->window.getWinowKey());

        LayoutTree::removeClean( l[ state.currentScreen ].layoutRoot
                               , selected );

        l[ state.currentScreen ].replace();
        state.currentScreen = nextScreen;

        return NeedRelayout;
    }

    ScreenMoveRotative::ScreenMoveRotative(int amount)
        : _moveAmount( amount )
    {}


    Action::ReturnInfo ScreenMoveRotative::operator()( DesktopLayout &l
                                                     , WindowMakerState &state )
    {
        return NeedRelayout;
    }
}}