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

    class InverseScreenLayout : public Action
    {
    public:

        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);
    };
}}

#endif /* SYSTEM_H */
