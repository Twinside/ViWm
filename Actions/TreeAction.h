#ifndef TREEACTION_H
#define TREEACTION_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class HorizontalSplit : public Action
    {
    public:
        virtual ReturnInfo   operator() ( DesktopLayout &l, WindowMakerState &state );
    };

    class VerticalSplit : public Action
    {
    public:
        virtual ReturnInfo   operator() ( DesktopLayout &l, WindowMakerState &state );
    };

    class NodeRotate : public Action
    {
    public:
        NodeRotate( int directionAmount );
        virtual ReturnInfo   operator() ( DesktopLayout &l
                                        , WindowMakerState &state );

    private:
        int amount;
    };
}}

#endif /* TREEACTION_H */
