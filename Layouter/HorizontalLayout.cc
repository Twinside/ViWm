#include "HorizontalLayout.h"

namespace Layout
{
#if 0
    void    horizontal()
    {
        if (i < masterarea_count) {
            /* Main window */
            x = (screen_width / masterarea_count) * i;
            y = 0;
            width = (screen_width / masterarea_count);
            height = (screen_height / 2) + margin;
        } else {
            /* Normal windows to be tiled */
            x = (screen_width / ((nodeCount + 1) - masterarea_count)) * (nodeCount - i);
            y = (screen_height / 2) + margin;
            width = (screen_width / ((nodeCount + 1) - masterarea_count));
            height = (screen_height / 2) - margin;
        }

            SetWindowPos(temp->hwnd, HWND_TOP, x + screen_x, y + screen_y, width, height, SWP_SHOWWINDOW);
    }
#endif

    void HorizontalLayout::addNewWindowToLayout( TilledWindow& /* newWindow */
                                               , const WindowMakerState& /* st */
                                               , DesktopLayout& /* l */ )
    {

    }

    void HorizontalLayout::layout( const WindowMakerState& /* s */
                                 , DesktopLayout& /* l */
                                 )
    {

    }
}
