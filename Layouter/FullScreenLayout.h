#ifndef FULLSCREENLAYOUT_H
#define FULLSCREENLAYOUT_H

#include "../Layouter.h"

namespace ViWm {
namespace Layout
{
    class FullScreenLayout : public Layouter
    {
    public:
        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         );
        virtual void layout( const WindowMakerState &s, DesktopLayout &l );
    };
}}

#endif /* FULLSCREENLAYOUT_H */