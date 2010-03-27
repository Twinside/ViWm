#ifndef TREEACTION_H
#define TREEACTION_H

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class HorizontalSplit : public Action
    {
    public:
        HorizontalSplit( StringId display, StringId descr );
        virtual ReturnInfo   operator() ( DesktopLayout &l, WindowMakerState &state );
    };

    class VerticalSplit : public Action
    {
    public:
        VerticalSplit( StringId display, StringId descr );
        virtual ReturnInfo   operator() ( DesktopLayout &l, WindowMakerState &state );
    };

    class NodeRotate : public Action
    {
    public:
        NodeRotate( StringId display, StringId descr
                  , int directionAmount );
        virtual ReturnInfo   operator() ( DesktopLayout &l
                                        , WindowMakerState &state );

    private:
        int amount;
    };

    class Relayout : public Action
    {
    public:
        Relayout( StringId display, StringId descr );
        virtual ReturnInfo  operator ()( DesktopLayout& l, WindowMakerState &s );
    };

    class VerticalMove : public Action
    {
    public:
        VerticalMove( StringId display, StringId descr
                    , IterationDirection amount );
        
        virtual ReturnInfo  operator ()( DesktopLayout& l
                                       , WindowMakerState &s );

    private:
        IterationDirection _moveAmount;
    };

    class HorizontalMove : public Action
    {
    public:
        HorizontalMove( StringId display, StringId descr
                      , IterationDirection amount );
        

        virtual ReturnInfo  operator ()( DesktopLayout& l
                                       , WindowMakerState &s );
    private:
        IterationDirection _moveAmount;
    };
}}

#endif /* TREEACTION_H */
