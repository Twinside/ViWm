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

        LayoutLeaf* selected = currentTree->getSelected();
        LayoutNode* parent = selected->getParent();

        // we need to check if we can relayout this node
        // because it might be deleted by the removeClen.
        bool        needRelayout = parent->getSubNodeCount() > 2;

        LayoutTree::addCreate( l[ nextScreen ].layoutRoot
                             , *new LayoutLeaf( selected->getWindow()) );

        l[ nextScreen ].layoutRoot->selectNode(
                selected->getWindow().getWinowKey());

        LayoutTree::removeClean( l[ state.currentScreen ].layoutRoot
                               , selected );

        // force update of the old screen
        if ( needRelayout )
            state.getCurrentLayouter().reLayout( l[ state.currentScreen ], *parent);
        else
            state.getCurrentLayouter().layout( state, l );
        l[ state.currentScreen ].replace( true );

        state.currentScreen = nextScreen;

        return NeedRelayout;
    }
}}
