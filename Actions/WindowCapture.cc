#include "WindowCapture.h"

namespace ViWm {
namespace Actions
{
    WindowCapture::WindowCapture( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    Action::ReturnInfo   WindowCapture::operator() ( DesktopLayout&    /*l*/
                                                   , WindowMakerState& /*state*/ )
    {
        return Nothing;
    }

    WindowRelease::WindowRelease( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    Action::ReturnInfo   WindowRelease::operator() ( DesktopLayout&    /*l*/
                                                   , WindowMakerState& /*state*/ )
    {
        return Nothing;
    }
}}


