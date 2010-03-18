#include <assert.h>
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
        if ( selected->getParent() == 0 )
        {
            LayoutNode  *n = new LayoutNode();

            n->addNode( selected );
            n->addNode( 0 );

            l[ state.currentScreen ].layoutRoot = n;
            l[ state.currentScreen ].initialSplit = goodSide;

            return;
        }

        LayoutNode  *parent = selected->getParent();

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

        LayoutLeaf *selected = root->getSelected();

        // it's an end node, so must add a new Node
        if ( !selected || selected->getParent() == 0 )
            return Nothing;

        selected->getParent()->rotate( 1 );
        selected->getParent()->selectNode( selected->getWindow().getWinowKey() );

        return NeedRelayout;
    }

    Action::ReturnInfo Relayout::operator ()( DesktopLayout&    /*l*/
                                            , WindowMakerState& /*s*/ )
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
        assert( amount != 0 );
    }

    LayoutNode* findWellOriented( LayoutTree::SplitSide direction, LayoutTree *src )
    {
        LayoutNode* parent = src->getParent();
        if ( parent == 0 ) return 0;

        if ( parent->getLastDirection() == direction )
            return parent;
        else return findWellOriented( direction, parent );
    }

    Action::ReturnInfo VerticalMove::operator ()( DesktopLayout& l, WindowMakerState &s )
    {
        LayoutTree* root = l[ s.currentScreen ].layoutRoot;
        if ( !root ) return Nothing;

        LayoutLeaf* selected = root->getSelected();
        if ( !selected ) return Nothing;

        const Rect& selectSize =
            selected->getParent() != 0
            ? selected->getParent()->getSelectedSize()
            : l[ s.currentScreen ].getSize();

        LayoutNode* goodParent =
            findWellOriented( LayoutTree::SplitHorizontal, selected );

        LayoutLeaf* nextSelection;

        // try each horizontal layer till we find a good one
        while ( goodParent )
        {
            if ( goodParent->moveSelection( _moveAmount ) )
                break;
            goodParent =
                findWellOriented( LayoutTree::SplitHorizontal, goodParent );
        }

        // if we didn't found a good horizontal layer
        if ( !goodParent )
            return Nothing;

        if ( _moveAmount < 0 )
            goodParent->pickNode( selectSize.x + selectSize.width / 2 
                                , selectSize.y - LayoutNode::SplitWidth - 1 );
        else
            goodParent->pickNode( selectSize.x + selectSize.width / 2
                                , selectSize.y + selectSize.height + LayoutNode::SplitWidth + 1 );

        // our next window, still in the same screen.
        nextSelection = goodParent->getSelected();

        if ( s.current )
            s.current->SetTransparency( s.getConf().idleTransparency() );

        if ( nextSelection )
        {
            s.current = &nextSelection->getWindow();
            s.current->GiveFocus();
        }

        return Nothing;
    }

    HorizontalMove::HorizontalMove(StringId display, StringId descr, int amount)
        : Action( display, descr ), _moveAmount( amount )
    {
        assert( amount != 0 );
    }

    Action::ReturnInfo HorizontalMove::operator ()( DesktopLayout& l
                                                  , WindowMakerState &s )
    {
        LayoutTree* root = l[ s.currentScreen ].layoutRoot;
        if ( !root ) return Nothing;

        LayoutLeaf* selected = root->getSelected();
        if ( !selected ) return Nothing;

        const Rect& selectSize =
            selected->getParent() != 0
            ? selected->getParent()->getSelectedSize()
            : l[ s.currentScreen ].getSize();

        LayoutNode* goodParent =
            findWellOriented( LayoutTree::SplitVertical, selected );

        LayoutLeaf* nextSelection;

        // try each vertical layer till we find one which can
        // move us
        while ( goodParent )
        {
            if ( goodParent->moveSelection( _moveAmount ) )
                break;

            goodParent = findWellOriented( LayoutTree::SplitVertical
                                         , goodParent );
        }

        if ( goodParent )
        {
            if ( _moveAmount < 0 )
                goodParent->pickNode( selectSize.x - LayoutNode::SplitWidth - 1
                                    , selectSize.y + selectSize.height / 2 );
            else
                goodParent->pickNode( selectSize.x + selectSize.width + LayoutNode::SplitWidth + 1
                                    , selectSize.y + selectSize.height / 2 );

            // our next window, still in the same screen.
            nextSelection = goodParent->getSelected();
        }
        // we didn't find a split with the good orientation
        // so, we want to give focus to another screen, if
        // any.
        else if ( _moveAmount < 0 ) // we are moving to the left =)
        {
            // we're already at the left margin, cannot
            // jump elsewhere
            if (int(s.currentScreen) + _moveAmount < 0)
                return Nothing;

            size_t  prevScreen = int(s.currentScreen) + _moveAmount;

            if ( !l[ prevScreen ].layoutRoot )
                return Nothing;

            int     pickHeight = selectSize.y + selectSize.height / 2
                               - l[ prevScreen ].getY();

            l[prevScreen].layoutRoot->pickNode( l[prevScreen].getX() + l[prevScreen].getWidth() - 1
                                              , pickHeight );

            nextSelection = l[prevScreen].layoutRoot->getSelected();
        }
        else // assume _moveAmount > 0
        {
            // we are at the right margin.
            if (s.currentScreen + size_t(_moveAmount) >= l.size())
                return Nothing;

            size_t nextScreen = s.currentScreen + size_t( _moveAmount );

            if ( !l[nextScreen].layoutRoot )
                return Nothing;

            int     pickHeight = selectSize.y + selectSize.height / 2
                               - l[ nextScreen ].getY();

            l[nextScreen].layoutRoot->pickNode( -1, pickHeight );
            nextSelection = l[nextScreen].layoutRoot->getSelected();
        }

        if ( s.current )
            s.current->SetTransparency( s.getConf().idleTransparency() );

        if ( nextSelection )
        {
            s.current = &nextSelection->getWindow();
            s.current->GiveFocus();
        }

        return Nothing;
    }
}}

