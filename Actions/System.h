#ifndef SYSTEM_H
#define SYSTEM_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class ExitAction : public Action
    {
    public:
        ExitAction( StringId display, StringId descr
                  , LayoutTree::WindowKey key );

        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        LayoutTree::WindowKey toSend;
    };

    class InverseScreenLayout : public Action
    {
    public:
        InverseScreenLayout( StringId display, StringId descr );

        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);
    };
}}

#endif /* SYSTEM_H */
