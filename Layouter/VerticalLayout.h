#ifndef VERTICALLAYOUT_H
#define VERTICALLAYOUT_H

#include "../Layouter.h"

namespace Layout
{
    class VerticalLayout : public Layouter
    {
    public:
        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         );

        virtual void layout( const WindowMakerState &s
                           , DesktopLayout &l );
    };
}

#endif /* VERTICALLAYOUT_H */