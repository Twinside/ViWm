#include "WindowCapture.h"

namespace ViWm {
namespace Actions
{
    WindowCapture::WindowCapture( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    ReturnInfo   WindowCapture::operator() ( DesktopLayout &l
                                           , WindowMakerState &state )
    {
    }

    WindowRelease::WindowRelease( StringId display
                                , StringId descr )
        : Action( display, descr )
    {}

    ReturnInfo   WindowRelease::operator() ( DesktopLayout &l
                                           , WindowMakerState &state )
    {
    }
}}


