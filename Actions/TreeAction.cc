#include "TreeAction.h"

namespace ViWm {
namespace Actions
{
    void    reSplit( LayoutTree::SplitSide goodSide
                   , DesktopLayout &l
                   , WindowMakerState &state )
    {
        LayoutTree  *root = l[ state.currentScreen ].layoutRoot;

        if ( ! root ) return;

        LayoutTree  *selected = root->getSelected();

        // it's an end node, so must add a new Node
        if ( selected->parent == 0 )
        {
            LayoutNode  *n = new LayoutNode();

            n->addNode( selected );
            n->addNode( 0 );

            l[ state.currentScreen ].layoutRoot = n;

            return;
        }

        LayoutNode  *parent = static_cast<LayoutNode*>( selected->parent );

        // already well splited, so add a new node.
        if (parent->getLastDirection() == goodSide)
            parent->insertBefore( selected, 0 );

        // not the good split side, so we must
        // subdivide the parent node.
        else
        {
            LayoutNode  *newNode = new LayoutNode();
            parent->replace( selected, newNode );

            // order is important
            newNode->addNode( selected );
            newNode->addNode( 0 );
        }
    }

    Action::ReturnInfo HorizontalSplit::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        reSplit( LayoutTree::SplitHorizontal, l, state );
        return NeedRelayout;
    }

    Action::ReturnInfo VerticalSplit::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        reSplit( LayoutTree::SplitVertical, l, state );
        return NeedRelayout;
    }

    NodeRotate::NodeRotate( int directionAmount )
        : amount( directionAmount )
    {}

    Action::ReturnInfo NodeRotate::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        LayoutTree  *root = l[ state.currentScreen ].layoutRoot;

        if ( ! root ) return Nothing;

        LayoutTree  *selected = root->getSelected();

        // it's an end node, so must add a new Node
        if ( selected->parent == 0 )
            return Nothing;

        static_cast<LayoutNode*>(selected->parent)->rotate( 1 );

        return NeedRelayout;
    }

    Action::ReturnInfo Relayout::operator ()( DesktopLayout& l, WindowMakerState &s )
    {
        /* TODO : implement here */
    }
}}

