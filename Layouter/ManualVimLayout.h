#ifndef MANUALVIMLAYOUT_H
#define MANUALVIMLAYOUT_H

#include "../Layouter.h"

namespace Layout
{
    class ManualVimLayout : public Layouter
    {
    public:
        virtual void layout( const WindowMakerState &s, DesktopLayout &l );
        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         );
    };
}

#endif /* MANUALVIMLAYOUT_H */
