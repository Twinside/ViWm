#include "FullScreenLayout.h"

namespace ViWm {
namespace Layout
{
    void FullScreenLayout::layout( const WindowMakerState& /* s */
                                 , DesktopLayout& /* l */ )
    {
#if 0
        std::list< TilledWindow* >::iterator    lst;
        std::list< TilledWindow* >::iterator    end = s.windowList[ s.currentTag ].end();

        for ( lst = s.windowList[s.currentTag].begin()
            ; lst != end ; ++lst )
        {
            (*lst)->SetSize( 0 + screen_x
                           , 0 + screen_y
                           , screen_width
                           , screen_height
                           );
        }
#endif
    }

    void FullScreenLayout::addNewWindowToLayout( TilledWindow& /* newWindow */
                                               , const WindowMakerState& /* st */
                                               , DesktopLayout& /* l */)
    {

    }
}}
