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
            if (cit->width > 0)
            {
                unconstrainedWidth -= cit->width;
                constrainedWidthCount++;
            }

            if (cit->height > 0)
            {
                unconstrainedHeight -= cit->height;
                constrainedHeightCount++;
            }
        }

        Rect        subSize;
        Rect        logicalSubsize;
        subSize = dim;
        logicalSubsize = dim;

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
                    subSize.height = it->height - sizeSub;
                    logicalSubsize.height = it->height;
                }
                else
                {
                    subSize.height = unconstrainedSize - sizeSub;
                    logicalSubsize.height = unconstrainedSize;
                }

                subSize.y += topShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitVertical );
                }
                it->lastScreenDim = subSize;
                it->lastLogicalDimension = logicalSubsize;

                subSize.y += subSize.height + HalfSplit;
                logicalSubsize.y += logicalSubsize.height;
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
                    subSize.width = it->width - sizeSub;
                    logicalSubsize.width = it->width;
                }
                else
                {
                    subSize.width = unconstrainedSize - sizeSub;
                    logicalSubsize.width = unconstrainedSize;
                }

                subSize.x += leftShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitHorizontal );
                }
                it->lastScreenDim = subSize;
                it->lastLogicalDimension = logicalSubsize;

                subSize.x += subSize.width + HalfSplit;
                logicalSubsize.x += logicalSubsize.width;
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
        { return SplitCoord(); }

    LayoutTree::SplitCoord LayoutNode::FindPointedSplit( int x, int y )
    {
        Collection::const_iterator it;
        size_t i = 0;

        assert( x >= 0 && y >= 0 );

        if ( lastDirection == SplitHorizontal )
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it, i++)
            {
                const Rect    &prev = it->lastScreenDim;

                if ( y < prev.y + prev.height )
                {
                    // we must recurse in the current block
                    if ( it->subTree )
                        return it->subTree->FindPointedSplit( x, y );
                    else // we didn't found it.
                        return SplitCoord();
                }
                else if ( y < (it+1)->lastScreenDim.y )
                {
                    int distToSplitCenter =
                        HalfSplit - (y - prev.y - prev.height);
                    // we are selected
                    return SplitCoord( this, i, 0, distToSplitCenter );
                }
                // else not
            }
        }
        else // SplitVertical
        {
            for (it = nodes.begin(); it != nodes.end() - 1; ++it, i++)
            {
                const Rect    &prev = it->lastScreenDim;

                if ( x < prev.x + prev.width )
                {
                    // we must recurse in the current block
                    if ( it->subTree )
                        return it->subTree->FindPointedSplit( x, y );
                    else // we didn't found it.
                        return SplitCoord();
                }
                else if ( x < (it+1)->lastScreenDim.x )
                {
                    int distToSplitCenter =
                        HalfSplit - (x - prev.x - prev.width);

                    // we are selected
                    return SplitCoord( this, i, distToSplitCenter, 0 );
                }
                // else not
            }
        }

        // if we haven't found anything...
        return SplitCoord();
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
                const Rect    &top = it->lastScreenDim;

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
                const Rect    &left = it->lastScreenDim;
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

            int desiredWidth = x - previous.lastLogicalDimension.x;

            // the size doesn't change, so we don't care
            if ( desiredWidth == previous.lastLogicalDimension.width )
                return false;

            // *ugh* we're asked to be very small, yet we can't be smaller
            // than "MinimumViewableSize"
            if ( desiredWidth < 0 )
            {
                // at the beginning of the box, we can't shrink
                // more.
                if ( splitIndex == 0 ) return false;

                if ( splitDeltaPropagate( current, splitIndex - 1
                                        , Backward, desiredWidth ) )
                {
                    nodes[splitIndex + 1].width += previous.lastLogicalDimension.width
                                                 - MinimumViewableSize
                                                 - desiredWidth;
                    previous.width = MinimumViewableSize;
                    return true;
                }
            }
            // if we already are at the minimum size, we can't be even
            // smaller. Or we can try to "push" previous windows.
            if ( desiredWidth < MinimumViewableSize )
            {
                if ( splitIndex == 0 ) return false;

                int diff = desiredWidth - MinimumViewableSize;

                if ( splitDeltaPropagate( current, splitIndex - 1
                                        , Backward, diff ) )
                {
                    nodes[splitIndex + 1].width += previous.lastLogicalDimension.width
                                                 - desiredWidth;
                    previous.width = MinimumViewableSize;
                    return true;
                }
            }
            // we are shrinking ourselves but we are in normal
            // operating range
            else if ( previous.lastLogicalDimension.width > desiredWidth )
            {
                nodes[splitIndex + 1].width += previous.lastLogicalDimension.width
                                             - desiredWidth;
                previous.width = desiredWidth;
                return true;
            }
            // Allright, we are expending ourselves, we just have
            // to make sure that there is enough place to expand
            // without violating the minimum size constraint.
            else if ( splitDeltaPropagate( current, splitIndex + 1
                                         , Forward, previous.width - desiredWidth ) )
            {
                // everything ok, commit the size change
                previous.width= desiredWidth;
                return true;
            }
        }
        else // SplitHorizontal
        {
            // if we are out of our bounds, we can't resize
            if ( y < prevDim.y || y >= prevDim.y + prevDim.height )
                return false;

            int desiredHeight = y - previous.lastLogicalDimension.y;

            // the size doesn't change, so we don't care
            if ( desiredHeight == previous.lastLogicalDimension.height )
                return false;

            // *ugh* we're asked to be very small, yet we can't be smaller
            // than "MinimumViewableSize"
            if ( desiredHeight < 0 )
            {
                // at the beginning of the box, we can't shrink
                // more.
                if ( splitIndex == 0 ) return false;

                if ( splitDeltaPropagate( current, splitIndex - 1
                                        , Backward, desiredHeight ) )
                {
                    nodes[splitIndex + 1].height += previous.lastLogicalDimension.height
                                                  - MinimumViewableSize
                                                  - desiredHeight;
                    previous.height = MinimumViewableSize;
                    return true;
                }
            }
            // if we already are at the minimum size, we can't be even
            // smaller. Or we can try to "push" previous windows.
            if ( desiredHeight < MinimumViewableSize )
            {
                if ( splitIndex == 0 ) return false;

                int diff = desiredHeight - MinimumViewableSize;

                if ( splitDeltaPropagate( current, splitIndex - 1
                                        , Backward, diff ) )
                {
                    nodes[splitIndex + 1].height += previous.lastLogicalDimension.height
                                                 - desiredHeight;
                    previous.height = MinimumViewableSize;
                    return true;
                }
            }
            // we are shrinking ourselves but we are in normal
            // operating range
            else if ( previous.lastLogicalDimension.height > desiredHeight )
            {
                nodes[splitIndex + 1].height += previous.lastLogicalDimension.height
                                              - desiredHeight;
                previous.height = desiredHeight;
                return true;
            }
            // Allright, we are expending ourselves, we just have
            // to make sure that there is enough place to expand
            // without violating the minimum size constraint.
            else if ( splitDeltaPropagate( current, splitIndex + 1
                                         , Forward, previous.height - desiredHeight ) )
            {
                // everything ok, commit the size change
                previous.height = desiredHeight;
                return true;
            }
        }

        return false;
    }

    bool LayoutNode::splitDeltaPropagate( const Screen &s
                                        , size_t splitIndex
                                        , IterationDirection dir
                                        , int           delta
                                        )
    {
        const int minSize = MinimumViewableSize;
        SizePair    &previous = nodes[splitIndex];

        if ( delta == 0 ) return true;

        if ( lastDirection == SplitVertical )
        {
            // we cannot satisfy the constraint.
            if ( previous.width + delta < minSize )
            {
                // if we are at the end of the box, we
                // cannot do anything
                if ( (int(dir) > 0 && splitIndex == nodes.size() - 1)
                    || (int(dir) < 0 && splitIndex == 0) )
                    return false;

                // we can safely assume that delta is negative. If it was positive
                // we couldn't be smaller than the minSize.
                int newProp = delta + (previous.lastLogicalDimension.width - minSize);

                // we can try to take the maximum of delta and forward the rest
                // to others.
                if ( splitDeltaPropagate( s, splitIndex + int(dir), dir, newProp ))
                {
                    previous.width= minSize;
                    return true;
                }
                else return false;
            }
            else
            {
                previous.width += delta;
                return true;
            }
        }
        else // SplitHorizontal
        {
            // we cannot satisfy the constraint.
            if ( previous.height + delta < minSize )
            {
                // if we are at the end of the box, we
                // cannot do anything
                if ( (int(dir) > 0 && splitIndex == nodes.size() - 1)
                    || (int(dir) < 0 && splitIndex == 0) )
                    return false;

                // we can safely assume that delta is negative. If it was positive
                // we couldn't be smaller than the minSize.
                int newProp = delta + (previous.lastLogicalDimension.height - minSize);

                // we can try to take the maximum of delta and forward the rest
                // to others.
                if ( splitDeltaPropagate( s, splitIndex + int(dir), dir, newProp ))
                {
                    previous.height = minSize;
                    return true;
                }
                else return false;
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
                return p->nodes[i].lastScreenDim;
        }

        // if we reach this code path, we clearly have a problem
        // and a degenerated tree.
        throw;
    }

    LayoutTree::SplitCoord::SplitCoord()
        : splitFather( NULL ) , splitIndex( 0 )
    {}

    LayoutTree::SplitCoord::SplitCoord( LayoutNode* father, size_t id
                                      , int deltaX, int deltaY )
        : splitFather( father )
        , splitIndex( id )
        , xDelta( deltaX )
        , yDelta( deltaY )
    {}
}
