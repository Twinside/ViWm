#ifndef MULTISCREEN_H
#define MULTISCREEN_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class ScreenMoverBounded : public Action
    {
    public:
        ScreenMoverBounded( StringId display
                          , StringId descr
                          , int amount );
        
        virtual ReturnInfo   operator()( DesktopLayout &l
                                       , WindowMakerState &state );
    private:
        int _moveAmount;
    };
}}

#endif /* MULTISCREEN_H */
