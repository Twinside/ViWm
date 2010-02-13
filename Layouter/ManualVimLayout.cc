#include "ManualVimLayout.h"
#include "../State.h"
#include "../Screen.h"

namespace ViWm {
namespace Layout
{
    void ManualVimLayout::layout( const WindowMakerState &s, DesktopLayout &l )
    { 
        /* This Layout does nothing by himself
         *
         */
    }

    void ManualVimLayout::addNewWindowToLayout( TilledWindow &newWindow
                                              , const WindowMakerState &st
                                              , DesktopLayout &l )
    {
        LayoutTree::addCreate( l[ st.currentScreen ].layoutRoot
                             , *new LayoutLeaf( newWindow ) );
    }
}}
