#ifndef LAYOUTER_H
#define LAYOUTER_H

#include "Screen.h"

namespace ViWm
{
    enum TillingMode
    {
        VerticalTilling,
        HorizontalTilling,
        FullScreenTilling,
        ManualVimTilling,
        LastTillingMode
    };

    class TilledWindow;
    class WindowMakerState;

    /**
     * Class in charge of organizing the windows on the
     * screen. Different algorithms can be used.
     */
    class Layouter
    {
    public:
        Layouter(void);
        virtual ~Layouter(void);

        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         ) = 0;
        /**
         * Role : modify the layout l in function of state s.
         * Hoped "functionnal type" :
         * WindowMakerState -> DesktopLayout
         * You also have to resize windows.
         */
        virtual void layout( const WindowMakerState &s, DesktopLayout &l ) = 0;
    };
}

#endif /* LAYOUTER_H */
