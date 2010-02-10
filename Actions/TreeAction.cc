
#include "TreeAction.h"

namespace Actions
{
    Action::ReturnInfo HorizontalSplit::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        return Nothing;
    }

    Action::ReturnInfo VerticalSplit::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        return Nothing;
    }

    NodeRotate::NodeRotate( int directionAmount )
        : amount( directionAmount )
    {}

    Action::ReturnInfo NodeRotate::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        return Nothing;
    }
}
