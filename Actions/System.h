#ifndef SYSTEM_H
#define SYSTEM_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class ExitAction : public Action
    {
    public:
        ExitAction( LayoutTree::WindowKey key );

        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        LayoutTree::WindowKey toSend;
    };
}}

#endif /* SYSTEM_H */
