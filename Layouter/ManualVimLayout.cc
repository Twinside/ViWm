#include "ManualVimLayout.h"
#include "../State.h"

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
        // no top level.
        if ( st.adressDepth <= 0 )
        {
            l[ st.currentScreen ].layoutRoot =
                new LayoutLeaf( &newWindow );
        }
        else
        {

        }
    }
}
