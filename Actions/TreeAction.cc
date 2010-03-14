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
            l[ state.currentScreen ].initialSplit = goodSide;

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

    HorizontalSplit::HorizontalSplit( StringId display, StringId descr )
        : Action( display, descr )
    {}

    Action::ReturnInfo VerticalSplit::operator()( DesktopLayout &l, WindowMakerState &state )
    {
        reSplit( LayoutTree::SplitVertical, l, state );
        return NeedRelayout;
    }

    VerticalSplit::VerticalSplit( StringId display, StringId descr )
        : Action( display, descr )
    {}

    NodeRotate::NodeRotate( StringId display, StringId descr
                          , int directionAmount )
        : Action( display, descr )
        , amount( directionAmount )
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
        /* yes, that's it */
        return NeedRelayout;
    }

    Relayout::Relayout( StringId display, StringId descr )
        : Action( display, descr )
    {}


    VerticalMove::VerticalMove(StringId display, StringId descr, int amount)
        : Action( display, descr ), _moveAmount( amount )
    {
    }

    Action::ReturnInfo VerticalMove::operator ()( DesktopLayout& l, WindowMakerState &s )
    {
        /* TODO : implement here */
        return Nothing;
    }

    HorizontalMove::HorizontalMove(StringId display, StringId descr, int amount)
        : Action( display, descr ), _moveAmount( amount )
    {}

    LayoutNode* findWellOriented( LayoutTree::SplitSide direction, LayoutTree *src )
    {
        LayoutNode* parent = static_cast<LayoutNode*>( src->parent );
        if ( parent == 0 ) return 0;

        if ( parent->getLastDirection() == direction )
            return parent;
        else return findWellOriented( direction, parent );
    }

    Action::ReturnInfo HorizontalMove::operator ()( DesktopLayout& l
                                                  , WindowMakerState &s )
    {
        LayoutTree* root = l[ s.currentScreen ].layoutRoot;
        if ( !root ) return Nothing;

        LayoutNode* goodParent =
            findWellOriented( LayoutTree::SplitVertical, root );

        if ( goodParent && goodParent->moveSelection( _moveAmount ) )
        {
            if ( s.current )
                s.current->GiveFocus();

            return Nothing;
        }
        // we didn't find a split with the good orientation
        // so, we want to give focus to another screen, if
        // any.
        if ( ! goodParent )
        {
        }
        else if (false)
        {

        }


        return Nothing;
    }
}}

