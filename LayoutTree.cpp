#include <assert.h>
#include <algorithm>
#include "LayoutTree.h"
#include "Screen.h"

namespace ViWm
{
    struct ChildFinder
    {
        LayoutTree  *node;
        ChildFinder ( LayoutTree *nnode ) : node( nnode ) {}

        bool   operator() ( LayoutNode::SizePair &p )
        { return p.subTree == node; }
    };

    void LayoutNode::insert( LayoutTree *toSearch, LayoutTree *toAdd, int plusMinus )
    {
        ChildFinder             comparer( toSearch );
        Collection::iterator    it;

        it = std::find_if( nodes.begin(), nodes.end(), comparer );

        if ( it == nodes.end() )
            nodes.push_back( toSearch );
        else
        {
            // our selected route has an index of + 1
            if ( std::distance( nodes.begin(), it ) <= int(selectedRoute) )
                selectedRoute += 1 - plusMinus;

            nodes.insert( it + plusMinus, toAdd );

            if ( toAdd )
                toAdd->parent = this;
        }
    }

    bool LayoutNode::replace( LayoutTree *toSearch, LayoutTree *replacent )
    {
        ChildFinder             finder( toSearch );
        Collection::iterator    it;

        it = std::find_if( nodes.begin(), nodes.end(), finder );

        toSearch->parent = NULL;

        if (it == nodes.end()) return false;
        it->subTree = replacent;
        it->subTree->parent = this;

        return true;
    }

    LayoutTree::LayoutTree()
        : parent( 0 )
    {}

    LayoutTree::~LayoutTree() {}

    LayoutTree::CompStatus LayoutTree::addCreate( LayoutTree *&root, LayoutTree &tree )
    {

        if ( root == 0 )
            root = &tree;
        else if (root->addNode( &tree ) == Todo)
        {
            LayoutTree  *prevRoot = root;
            root = new LayoutNode();
            root->addNode( prevRoot );
            root->addNode( &tree );
        }

        return Done;
    }

    LayoutTree::CompStatus LayoutTree::removeClean( LayoutTree *&root, WindowKey key )
    {
        if ( root == 0 ) return Done;

        return globalPack( root, root->removeNode( key ) );
    }

    LayoutTree::CompStatus LayoutTree::removeClean( LayoutTree *&root, LayoutTree *tree )
    {
        if ( root == 0 ) return Done;
        return globalPack( root, root->removeNode( tree ) );
    }

    LayoutTree::CompStatus LayoutTree::globalPack( LayoutTree *&root, CompStatus st )
    {
        LayoutNode  *child;

        switch ( st )
        {
        case Todo:
            delete root;
            root = 0;
            break;

        case Compact:
            child = static_cast<LayoutNode*>(root);
            root = child->getFirstNode();
            if ( root )
                root->parent = 0;

            child->releaseChildren();
            delete child;
            break;

        case Searching: return Done;
        case Done: return Done;
        }
        return Done;
    }

    LayoutNode::~LayoutNode()
    {
        for ( Collection::iterator it = nodes.begin()
            ; it != nodes.end()
            ; ++it )
        {
            delete it->subTree;
        }
    }

    LayoutNode::LayoutNode()
        : selectedRoute( 0 )
    {}

    //////////////////////////////////////////////////////////////////////////
    ////                    Tree directed layout
    //////////////////////////////////////////////////////////////////////////
    void LayoutLeaf::Establish( const Screen& currentScreen
                              , const Rect &dim
                              , SplitSide /* side */ )
    {
        window.SetSize( dim.x + currentScreen.getX()
                      , dim.y + currentScreen.getY()
                      , dim.width
                      , dim.height );
    }

    void LayoutNode::Establish( const Screen &currentScreen
                              , const Rect &dim
                              , SplitSide side )

    {
        Collection::const_iterator cit;
        Collection::iterator       it;

        assert( nodes.size() > 1 );

        lastDirection = side;

        // first we need do collect some size constraints.
        int unconstrainedWidth = dim.width;
        int unconstrainedHeight = dim.height;
        size_t constrainedWidthCount = 0;
        size_t constrainedHeightCount = 0;

        for (cit = nodes.begin(); cit != nodes.end(); ++cit )
        {
            int realWidth = int(cit->width * currentScreen.getWidth());
            int realHeight = int(cit->height * currentScreen.getHeight());

            if (cit->width > 0)
            {
                unconstrainedWidth -= realWidth;
                constrainedWidthCount++;
            }

            if (cit->height > 0)
            {
                unconstrainedHeight -= realHeight;
                constrainedHeightCount++;
            }
        }

        Rect        subSize;
        subSize = dim;

        // during the second pass, we give real sizes to nodes
        //
        // +-----------------------------+
        // |   First window              |
        // |                             |
        // +-----------------------------+              ^
        // |    Separator                |-^            |  SplitWidth
        // +-----------------------------+ |            v
        // |                             | |  Height
        // |                             | |
        // |       Window                | |
        // |                             | |
        // +-----------------------------+ |
        // |    Separator                | x
        // +-----------------------------+ v    HalfSplit
        // |                             |
        // |     OtherWindow             |
        // |                             |
        // +-----------------------------+
        //
        //
        if ( side == SplitHorizontal )
        {
            int unconstrainedSize;

            if (constrainedHeightCount > 0 && constrainedHeightCount < nodes.size())
            {
                unconstrainedSize = unconstrainedHeight
                                  / (nodes.size() - constrainedHeightCount);
            }
            else
            {
                unconstrainedSize = dim.height
                                  / static_cast<int>( nodes.size() );
            }

            size_t i = 0;
            for (it = nodes.begin(); it != nodes.end(); ++it, i++)
            {
                int sizeSub = SplitWidth;
                int topShift = HalfSplit;

                if (i == 0)
                {
                    sizeSub = HalfSplit;
                    topShift = 0;
                } 
                else if (i == nodes.size() - 1)
                {
                    sizeSub = HalfSplit;
                }

                if ( it->height )
                {
                    int realHeight = int(it->height * currentScreen.getHeight());
                    subSize.height = realHeight - sizeSub;
                }
                else
                    subSize.height = unconstrainedSize - sizeSub;

                subSize.y += topShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitVertical );
                }
                it->lastDim = subSize;
                subSize.y += subSize.height + HalfSplit;
            }
        }
        else // SplitVertical
        {
            int unconstrainedSize;
            
            if ( constrainedWidthCount > 0 && constrainedWidthCount < nodes.size())
            {
                unconstrainedSize = unconstrainedWidth 
                                  / (nodes.size() - constrainedWidthCount);
            }
            else
            {
                unconstrainedSize = dim.width
                                  / static_cast<int>( nodes.size() );
            }

            size_t i = 0;

            for (it = nodes.begin(); it != nodes.end(); ++it, i++)
            {
                int sizeSub = SplitWidth;
                int leftShift = HalfSplit;

                if (i == 0)
                {
                    sizeSub = HalfSplit;
                    leftShift = 0;
                }
                else if (i != nodes.size() - 1)
                    sizeSub = HalfSplit;

                if ( it->width )
                {
                    int realWidth = int(it->width * currentScreen.getWidth());
                    subSize.width = realWidth - sizeSub;
                }
                else
                    subSize.width = unconstrainedSize - sizeSub;

                subSize.x += leftShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitHorizontal );
                }
                it->lastDim = subSize;
                subSize.x += subSize.width + HalfSplit;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    ////                    Node adding
    //////////////////////////////////////////////////////////////////////////

    // idea of the algorithm, we go down to the the selected node
    // when we found the selected leaf, we backtrack once and add
    // the node there.
    LayoutTree::CompStatus LayoutNode::addNode( LayoutTree *window )
    {
        if ( nodes.size() == 0
            || (nodes[selectedRoute].subTree &&
                nodes[selectedRoute].subTree->addNode( window ) == Todo) )
        {
            nodes.push_back( window );
        }

        if ( window )
            window->parent = this;

        assert( nodes.size() >= 1 );
        return Done;
    }

    LayoutTree::CompStatus LayoutLeaf::addNode( LayoutTree* /*window*/ )
    { return Todo; }

    //////////////////////////////////////////////////////////////////////////
    ////                    Node Removing
    //////////////////////////////////////////////////////////////////////////

    // node removing is done in a BFS way.
    LayoutTree::CompStatus LayoutLeaf::removeNode( LayoutTree* toRemove )
        { return toRemove == this ? Todo : Done; }

    LayoutTree::CompStatus LayoutNode::removeNode( LayoutTree *toRemove )
    {
        struct SizePairComp
        {
            LayoutTree  *node;
            SizePairComp( LayoutTree *nnode ) : node( nnode ) {}

            bool   operator() ( LayoutNode::SizePair &p )
            {
                if ( p.subTree == node )
                {
                    delete p.subTree;
                    p.subTree = 0;
                    return true;
                }
                return false;
            }
        }   comparer( toRemove );

        assert( nodes.size() > 1 );

        nodes.erase( std::remove_if( nodes.begin(), nodes.end(), comparer ), nodes.end() );

        for ( size_t i = 0; i < nodes.size(); ++i )
            if ( nodes[i].subTree )
                pack( nodes[i].subTree->removeNode( toRemove ), i );

        // update the selected route into valid range
        // to avoid problems.
        if ( selectedRoute >= nodes.size() )
            selectedRoute = nodes.size() - 1;

        if ( nodes.size() == 0 ) return Todo;
        if ( nodes.size() == 1 ) return Compact;
        return Done;
    }

    // Depth First Search + same idea of backtracking as in node adding.
    LayoutTree::CompStatus LayoutLeaf::removeNode( WindowKey toRemove )
    { return window == toRemove ? Todo : Searching; }

    LayoutTree::CompStatus LayoutNode::removeNode( WindowKey toRemove )
    {
        CompStatus              lastOperation;
        assert( nodes.size() > 1 );

        for ( size_t i = 0; i < nodes.size(); i++ )
        {
            if ( nodes[i].subTree )
            {
                lastOperation = pack( nodes[i].subTree->removeNode( toRemove ), i );
                if ( lastOperation != Searching  )
                    return lastOperation;
            }
        }

        return Searching;
    }

    LayoutTree::CompStatus LayoutNode::pack( CompStatus what, size_t &i )
    {
        LayoutNode              *child;

        switch ( what )
        {
        case Todo:
            nodes.erase( nodes.begin() + i );

            if ( nodes.size() == 0 )
                return Todo;
            else if ( nodes.size() == 1 )
                return Compact;

            i--;
            return Done;

        case Compact:
            child = static_cast<LayoutNode*>(nodes[i].subTree);

            nodes[i].subTree = child->getFirstNode();

            if ( nodes[i].subTree )
                nodes[i].subTree->parent = this;

            child->releaseChildren();
            delete child;

            return Done;

        case Searching: return Searching;
        case Done: return Done;
        }

        return Done;
    }

    //////////////////////////////////////////////////////////////////////////
    ////                   Node selection
    //////////////////////////////////////////////////////////////////////////
    // it's a basic Depth First Search.
    // Set up the selectedRoute to point to the good direction.
    LayoutTree::CompStatus LayoutLeaf::selectNode( WindowKey toSelect )
    { return window == toSelect ? Todo : Searching; }

    LayoutTree::CompStatus LayoutNode::selectNode( WindowKey toSelect )
    {
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if ( nodes[i].subTree
                && nodes[i].subTree->selectNode( toSelect ) != Searching )
            {
                selectedRoute = i;
                return Done;
            }
        }

        return Searching;
    }


    // with a bit of luck, it'l even be tail recursive.
    LayoutTree* LayoutLeaf::getSelected()
    { return this; }

    LayoutTree* LayoutNode::getSelected()
    { 
        if ( ! nodes[selectedRoute].subTree )
            return 0;

        return nodes[selectedRoute].subTree->getSelected();
    }

    LayoutTree* LayoutNode::getFirstNode()
    {
        if (nodes.size() < 1) return 0;

        return nodes[0].subTree;
    }

    void LayoutNode::releaseChildren()
    {
        for (size_t i = 0; i < nodes.size(); ++i)
            nodes[i].subTree = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    ////                        Layout Retrieving
    //////////////////////////////////////////////////////////////////////////
    LayoutTree::SplitCoord LayoutLeaf::FindPointedSplit( int /*x*/, int /*y*/ )
        { return SplitCoord( NULL, 0 ); }

    LayoutTree::SplitCoord LayoutNode::FindPointedSplit( int x, int y )
    {
        Collection::const_iterator it;
        size_t i = 0;

        assert( x >= 0 && y >= 0 );

        if ( lastDirection == SplitHorizontal )
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it, i++)
            {
                const Rect    &prev = it->lastDim;

                if ( y < prev.y + prev.height )
                {
                    // we must recurse in the current block
                    if ( it->subTree )
                        return it->subTree->FindPointedSplit( x, y );
                    else // we didn't found it.
                        return SplitCoord( NULL, 0 );
                }
                else if ( y < (it+1)->lastDim.y )
                {
                    // we are selected
                    return SplitCoord( this, i );
                }
                // else not
            }
        }
        else // SplitVertical
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it, i++)
            {
                const Rect    &prev = it->lastDim;

                if ( x < prev.x + prev.width )
                {
                    // we must recurse in the current block
                    if ( it->subTree )
                        return it->subTree->FindPointedSplit( x, y );
                    else // we didn't found it.
                        return SplitCoord( NULL, 0 );
                }
                else if ( x < (it+1)->lastDim.x )
                {
                    // we are selected
                    return SplitCoord( this, i );
                }
                // else not
            }
        }

        // if we haven't found anything...
        return SplitCoord( NULL, 0 );
    }
    
    //////////////////////////////////////////////////////////////////////////
    ////                        Layout drawing
    //////////////////////////////////////////////////////////////////////////
    void LayoutLeaf::displayLayoutStructure
                            ( Renderer::RenderWindow &/*r*/
                            , Renderer::RenderWindow::Brush /*defaultBrush*/ ) const {}
    void LayoutNode::displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::RenderWindow::Brush defaultBrush ) const
    {
        Collection::const_iterator it;

        if ( lastDirection == SplitHorizontal )
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it)
            {
                const Rect    &top = it->lastDim;

                r.drawRect( defaultBrush
                          , top.x
                          , top.y + top.height
                          , top.width
                          , SplitWidth );
            }
        }
        else
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it)
            {
                const Rect    &left = it->lastDim;
                r.drawRect( defaultBrush
                          , left.x + left.width
                          , left.y
                          , SplitWidth
                          , left.height );
            }
        }

        for (it = nodes.begin(); it != nodes.end(); ++it)
            if (it->subTree)
                it->subTree->displayLayoutStructure( r, defaultBrush );
    }


    bool LayoutNode::Iter( IteratingPredicate &p )
    {
        for ( Collection::iterator it = nodes.begin()
            ; it != nodes.end()
            ; ++it )
        {
            bool    status = p( *it );
            if ( status )
                return true;
        }

        return false;
    }

    bool LayoutNode::FocusTopIteration(IteratingPredicate &p)
    {
        LayoutNode  *sub = dynamic_cast<LayoutNode*>( nodes[ selectedRoute ].subTree );

        if ( sub && sub->FocusTopIteration( p ))
            return true;
        else if ( p( nodes[selectedRoute] ) )
            return true;

        return Iter( p );
    }

    bool LayoutNode::DepthFirstIteration(IteratingPredicate &p)
    {
        for ( Collection::iterator it = nodes.begin()
            ; it != nodes.end()
            ; ++it )
        {
            LayoutNode  *sub = dynamic_cast<LayoutNode*>(it->subTree);

            if ( (sub && sub->DepthFirstIteration( p ))
                || p( *it ) )
                return true;
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    ////                  Interface drag'n drop
    //////////////////////////////////////////////////////////////////////////
    bool    LayoutNode::moveSplit( const Screen &current
                                 , int x, int y
                                 , size_t splitIndex )
    {
        SizePair    &previous = nodes[splitIndex];
        Rect        prevDim = getMyPreviousDimension( current );

        if ( lastDirection == SplitVertical )
        {
            // if we are out of our bounds, we can't resize
            if ( x < prevDim.x || x >= prevDim.x + prevDim.width )
                return false;

            int delta = x - previous.lastDim.x;

            // if we already are at the minimum size, we can't be even
            // smaller. Or we can try to "push" previous windows.
            if ( delta < MinimumViewableSize )
            {
                if ( splitIndex == 0 ) return false;

                float newWidth = float(MinimumViewableSize)
                               / float(current.getWidth());

                float diff = delta / float(current.getWidth())
                           - newWidth;

                if ( splitDeltaPropagate
                            ( current
                            , splitIndex - 1
                            , Backward
                            , diff
                            ) )
                {
                    previous.width = newWidth;
                    nodes[splitIndex + 1].width -= diff;
                    return true;
                }

                return false;
            }

            float newWidth = delta / float( current.getWidth() );

            // Allright, we are expending ourselves, we just have
            // to make sure that there is enough place to expand
            // without violating the minimum size constraint.
            if ( splitDeltaPropagate
                        ( current
                        , splitIndex + 1
                        , Forward
                        , previous.width - newWidth
                        ) )
            {
                // everything ok, commit the size change
                previous.width = newWidth;
                return true;
            }
        }
        else // SplitHorizontal
        {
            // if we are out of our bounds, we can't resize
            if ( y < prevDim.y || y >= prevDim.y + prevDim.height )
                return false;

            int delta = y - previous.lastDim.y;

            // if we already are at the minimum size, we can't be even
            // smaller. Or we can try to "push" previous windows.
            if ( delta < MinimumViewableSize )
            {
                if ( splitIndex == 0 ) return false;

                float newHeight = float(MinimumViewableSize)
                                / float(current.getHeight());

                float diff = delta / float(current.getHeight())
                           - newHeight;

                if ( splitDeltaPropagate
                            ( current
                            , splitIndex - 1
                            , Backward
                            , diff
                            ) )
                {
                    previous.height = newHeight;
                    nodes[splitIndex + 1].height -= diff;
                    return true;
                }

                return false;
            }

            float newHeight = delta / float( current.getHeight() );

            // Allright, we are expending ourselves, we just have
            // to make sure that there is enough place to expand
            // without violating the minimum size constraint.
            if ( splitDeltaPropagate
                        ( current
                        , splitIndex + 1
                        , Forward
                        , previous.height - newHeight
                        ) )
            {
                // everything ok, commit the size change
                previous.height = newHeight;
                return true;
            }
        }

        return false;
    }

    bool LayoutNode::splitDeltaPropagate( const Screen &s
                                        , size_t splitIndex
                                        , IterationDirection dir
                                        , float  delta
                                        )
    {
        SizePair    &previous = nodes[splitIndex];

        if ( lastDirection == SplitVertical )
        {
            const float minSize = MinimumViewableSize / float(s.getWidth());

            // we cannot satisfy the constraint.
            if ( previous.width + delta < minSize ) 
            {
                // if we are at the end of the box, we
                // cannot do anything
                if ( (int(dir) > 0 && splitIndex == nodes.size() - 1)
                    || (int(dir) < 0 && splitIndex == 0) )
                    return false;

                // if we got followers, we can try to propagate
                // the loss
                return splitDeltaPropagate( s, splitIndex + int(dir), dir, delta );
            }
            else
            {
                previous.width += delta;
                return true;
            }
        }
        else // SplitHorizontal
        {
            const float minSize = MinimumViewableSize / float(s.getHeight());

            // we cannot satisfy the constraint.
            if ( previous.height + delta < minSize ) 
            {
                // if we are at the end of the box, we
                // cannot do anything
                if ( (int(dir) > 0 && splitIndex == nodes.size() - 1)
                    || (int(dir) < 0 && splitIndex == 0) )
                    return false;

                // if we got followers, we can try to propagate
                // the loss
                return splitDeltaPropagate( s, splitIndex + int(dir), dir, delta );
            }
            else
            {
                previous.height += delta;
                return true;
            }
        }
    }

    Rect LayoutNode::getMyPreviousDimension( const Screen &current ) const
    {
        if ( parent == 0 )
            return current.getSize();

        LayoutNode  *p = static_cast<LayoutNode*>(parent);

        for (size_t i = 0; i < p->nodes.size(); i++)
        {
            if ( p->nodes[i].subTree == this )
                return p->nodes[i].lastDim;
        }

        // if we reach this code path, we clearly have a problem
        // and a degenerated tree.
        throw;
    }
}
