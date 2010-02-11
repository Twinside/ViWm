#include "VerticalLayout.h"

namespace Layout
{
#if 0
    void    vertical()
    {
        if (i < masterarea_count) {
            x = 0;
            y = (screen_height / masterarea_count) * i;
            width = (screen_width / 2) + margin;
            height = (screen_height / masterarea_count);
        } else {
            x = (screen_width / 2) + margin;
            y = (screen_height / ((nodeCount + 1) - masterarea_count)) * (nodeCount - i);
            width = (screen_width / 2) - margin;
            height = (screen_height / ((nodeCount + 1) - masterarea_count));
        }
        SetWindowPos(temp->hwnd, HWND_TOP, x + screen_x, y + screen_y, width, height, SWP_SHOWWINDOW);
    }
#endif

    void VerticalLayout::addNewWindowToLayout( TilledWindow&    /* newWindow */
                                             , const WindowMakerState& /* st */
                                             , DesktopLayout& /* l */ )
    {

    }

    void VerticalLayout::layout( const WindowMakerState& /*s*/
                               , DesktopLayout& /* l */ )
    {

    }
}
