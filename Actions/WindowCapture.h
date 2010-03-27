#ifndef __WINDOWCAPTURE_H__
#define __WINDOWCAPTURE_H__

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class WindowCapture : public Action
    {
    public:
        WindowCapture( StringId display, StringId descr );
        
        virtual ReturnInfo   operator() ( DesktopLayout &l
                                        , WindowMakerState &state );
    private:
    };

    class WindowRelease : public Action
    {
    public:
        WindowRelease( StringId display, StringId descr );
        
        virtual ReturnInfo   operator() ( DesktopLayout &l
                                        , WindowMakerState &state );
    private:
    };
}}

#endif /* __WINDOWCAPTURE_H__ */

