#include "MultiScreen.h"

namespace ViWm {
namespace Actions
{

    ScreenMoverBounded::ScreenMoverBounded(int amount)
        : _moveAmount( amount )
    {}

    Action::ReturnInfo ScreenMoverBounded::operator()( DesktopLayout &l
                                                     , WindowMakerState &state )
    {
        return NeedRelayout;
    }

    ScreenMoveRotative::ScreenMoveRotative(int amount)
        : _moveAmount( amount )
    {}


    Action::ReturnInfo ScreenMoveRotative::operator()( DesktopLayout &l
                                                     , WindowMakerState &state )
    {
        return NeedRelayout;
    }
}}