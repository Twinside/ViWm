#ifndef MULTISCREEN_H
#define MULTISCREEN_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class ScreenMoverBounded : public Action
    {
    public:
        ScreenMoverBounded( int amount );
        
        virtual ReturnInfo   operator()( DesktopLayout &l
                                       , WindowMakerState &state );
    private:
        int _moveAmount;
    };

    class ScreenMoveRotative : public Action
    {
    public:
        ScreenMoveRotative( int amount );
        
        virtual ReturnInfo   operator()( DesktopLayout &l
                                       , WindowMakerState &state );
    private:
        int _moveAmount;
    };
}}

#endif /* MULTISCREEN_H */
