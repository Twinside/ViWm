#ifndef HORIZONTALLAYOUT_H
#define HORIZONTALLAYOUT_H

#include "../Layouter.h"

namespace ViWm {
namespace Layout
{
    class HorizontalLayout : public Layouter
    {
    public:
        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         );

        virtual void layout( const WindowMakerState &s
                           , DesktopLayout &l );
    };
}}

#endif /* HORIZONTALLAYOUT_H */