#include <assert.h>
#include <algorithm>
#include <xutility>
#include <iostream>
#include "LayoutTree.h"
#include "Screen.h"
#include "TilledWindow.h"


#ifdef _DEBUG
#   define INV_CHECK assert(checkInvariant())
#else /* DEBUG */
#   define INV_CHECK
#endif /* DEBUG */

#undef max
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
        INV_CHECK;
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
        INV_CHECK;
    }

    bool LayoutNode::replace( LayoutTree *toSearch, LayoutTree *replacent )
    {
        INV_CHECK;
        ChildFinder             finder( toSearch );
        Collection::iterator    it;

        it = std::find_if( nodes.begin(), nodes.end(), finder );

        toSearch->parent = NULL;

        if (it == nodes.end())
        {
            INV_CHECK;
            return false;
        }
        it->subTree = replacent;
        it->subTree->parent = this;

        INV_CHECK;
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

        assert( root->checkInvariant() );
        return Done;
    }

    LayoutTree::CompStatus LayoutTree::removeClean( LayoutTree *&root, WindowKey key )
    {
        if ( root == 0 ) return Done;

        CompStatus st = globalPack( root, root->removeNode( key ) );
        assert( !root || root->checkInvariant() );
        return st;
    }

    LayoutTree::CompStatus LayoutTree::removeClean( LayoutTree *&root, LayoutTree *tree )
    {
        if ( root == 0 ) return Done;
        CompStatus st = globalPack( root, root->removeNode( tree ) );
        assert( root->checkInvariant() );
        return st;
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

        case Searching: break;
        case Done: break;
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
        INV_CHECK;
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
        int constrainedDim;
        int dimSize;

        int &sizeWriter = ( side == SplitHorizontal )
                        ? subSize.height : subSize.width;

        int &logicalSizeWriter = ( side == SplitHorizontal )
                               ? logicalSubsize.height : subSize.width;

        int &startWriter = ( side == SplitHorizontal )
                         ? subSize.y : subSize.x;

        int &startLogicalWriter = ( side == SplitHorizontal )
                                ? logicalSubsize.y : logicalSubsize.x;

        if ( side == SplitHorizontal )
        {
            constrainedDim = constrainedHeightCount;
            dimSize = dim.height;
        }
        else
        {
            constrainedDim = constrainedWidthCount;
            dimSize = dim.width;
        }

        int unconstrainedSize;

        if (constrainedDim > 0 && constrainedDim < int(nodes.size()))
        {
            unconstrainedSize = unconstrainedHeight
                              / (int(nodes.size()) - constrainedDim);
        }
        else
        {
            unconstrainedSize = dimSize
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
                /// AAAAAAAAAAAAAAAARGH
                sizeWriter = it->height - sizeSub;
                logicalSizeWriter = it->height;
            }
            else
            {
                sizeWriter = unconstrainedSize - sizeSub;
                logicalSizeWriter = unconstrainedSize;
            }

            startWriter += topShift;
            if ( it->subTree )
            {
                it->subTree->Establish( currentScreen
                                      , subSize
                                      , SplitVertical );
            }
            it->lastScreenDim = subSize;
            it->lastLogicalDimension = logicalSubsize;

            startWriter += sizeWriter + HalfSplit;
            startLogicalWriter += logicalSizeWriter;
        }
        INV_CHECK;
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

        INV_CHECK;
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

    // for a reason I don't understand GCC refuse to instantiate
    // remove_if when SizePairComp is declared in removeNode
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
    };

    LayoutTree::CompStatus LayoutNode::removeNode( LayoutTree *toRemove )
    {
        INV_CHECK;
        SizePairComp comparer( toRemove );

        assert( nodes.size() > 1 );

        Collection::iterator it = 
            std::remove_if<Collection::iterator,SizePairComp>
                          ( nodes.begin(), nodes.end(), comparer );

        nodes.erase( it, nodes.end() );

        for ( size_t i = 0; i < nodes.size(); ++i )
            if ( nodes[i].subTree )
                pack( nodes[i].subTree->removeNode( toRemove ), i );

        // update the selected route into valid range
        // to avoid problems.
        if ( selectedRoute >= nodes.size() )
            selectedRoute = nodes.size() - 1;

        INV_CHECK;
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
                {
                    INV_CHECK;
                    return lastOperation;
                }
            }
        }

        INV_CHECK;
        return Searching;
    }

    LayoutTree::CompStatus LayoutNode::pack( CompStatus what, size_t &i )
    {
        LayoutNode              *child;

        // what about selection?
        switch ( what )
        {
        case Todo:
            nodes.erase( nodes.begin() + i );

            if ( nodes.size() == 0 )
                return Todo;
            else if ( nodes.size() == 1 )
                return Compact;

            i--;
            break;

        case Compact:
            child = static_cast<LayoutNode*>(nodes[i].subTree);

            nodes[i].subTree = child->getFirstNode();

            if ( nodes[i].subTree )
                nodes[i].subTree->parent = this;

            child->releaseChildren();
            delete child;
            break;

        case Searching: 
            return Searching;
        
        case Done: break;
        }

        // no invariant check because it's often called
        // by internal functions, thus not finished sabilizing
        // the internal state
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
        INV_CHECK;
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if ( nodes[i].subTree
                && nodes[i].subTree->selectNode( toSelect ) != Searching )
            {
                selectedRoute = i;
                INV_CHECK;
                return Done;
            }
        }

        INV_CHECK;
        return Searching;
    }


    // with a bit of luck, it'l even be tail recursive.
    LayoutLeaf* LayoutLeaf::getSelected()
    { return this; }

    LayoutLeaf* LayoutNode::getSelected()
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

        // hmm hard to insert an ending in variant check
        INV_CHECK;
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

            if ( y > nodes[i].lastScreenDim.y && nodes[i].subTree )
                return nodes[i].subTree->FindPointedSplit( x, y );
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

            if ( x > nodes[i].lastScreenDim.x && nodes[i].subTree )
                return nodes[i].subTree->FindPointedSplit( x, y );
        }

        // if we haven't found anything...
        return SplitCoord();
    }
    
    //////////////////////////////////////////////////////////////////////////
    ////                        Layout drawing
    //////////////////////////////////////////////////////////////////////////
    void LayoutLeaf::displayLayoutStructure
                            ( Renderer::RenderWindow &/*r*/
                            , Renderer::Brush /*defaultBrush*/ ) const {}

    void LayoutNode::displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::Brush defaultBrush ) const
    {
        Collection::const_iterator it;

        INV_CHECK;
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

        INV_CHECK;
    }


    bool LayoutNode::Iter( IteratingPredicate &p )
    {
        INV_CHECK;
        for ( Collection::iterator it = nodes.begin()
            ; it != nodes.end()
            ; ++it )
        {
            bool    status = p( *this, *it );
            if ( status )
            {
                INV_CHECK;
                return true;
            }
        }

        INV_CHECK;
        return false;
    }

    bool LayoutNode::FocusTopIteration(IteratingPredicate &p)
    {
        LayoutNode  *sub = dynamic_cast<LayoutNode*>( nodes[ selectedRoute ].subTree );

        INV_CHECK;
        bool answer;

        if ( sub && sub->FocusTopIteration( p )
            || p( *this, nodes[selectedRoute] ) )
        {
            answer = true;
        }
        else 
            answer = Iter( p );

        INV_CHECK;
        return answer;
    }

    bool LayoutNode::DepthFirstIteration(IteratingPredicate &p)
    {
        INV_CHECK;
        for ( Collection::iterator it = nodes.begin()
            ; it != nodes.end()
            ; ++it )
        {
            LayoutNode  *sub = dynamic_cast<LayoutNode*>(it->subTree);

            if ( (sub && sub->DepthFirstIteration( p ))
                || p( *this, *it ) )
            {
                INV_CHECK;
                return true;
            }
        }

        INV_CHECK;
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    ////                  Interface drag'n drop
    //////////////////////////////////////////////////////////////////////////
    bool    LayoutNode::moveSplit( const Screen &current
                                 , int x, int y
                                 , size_t splitIndex )
    {
        INV_CHECK;
        SizePair    &previous = nodes[splitIndex];
        Rect        prevDim = getMyPreviousDimension( current );

        // calculate all the subsize splits :]
        computMinimumSize();

        // it's ugly, but I can't put them inside the if
        // due to reference initialisation :]
        int&    previousDim =  (lastDirection == SplitVertical)
                            ? prevDim.width
                            : prevDim.height;

        int&    prevPoint = (lastDirection == SplitVertical)
                          ? prevDim.x
                          : prevDim.y;

        // redirection variables, used to avoid two version
        // (one for vertical & horizontal split)
        int     currPoint;
        int     lastPoint;
        int     lastSize;

        if ( lastDirection == SplitVertical )
        {
            currPoint = x;
            lastPoint = previous.lastLogicalDimension.x;
            lastSize = previous.lastLogicalDimension.width;
        }
        else
        {
            currPoint = y;
            lastPoint = previous.lastLogicalDimension.y;
            lastSize = previous.lastLogicalDimension.height;
        }

        // if we are out of our bounds, we can't resize
        if ( currPoint < prevPoint || currPoint >= prevPoint + previousDim )
            return false;   // TODO : fix this, doesn't work when root
                            // is the screen :(

        int desiredSize = currPoint - lastPoint;
        int sizeDelta = desiredSize - lastSize;

        // the size doesn't change, so we don't care
        if ( sizeDelta == 0 )
            return false;

        bool    canMoveBackward =
             canPropagateSplit( current, splitIndex
                              , Backward, lastDirection
                              , sizeDelta );

        bool    canMoveForward = canMoveBackward &&
             canPropagateSplit( current, splitIndex + 1
                              , Forward, lastDirection
                              , - sizeDelta );

        // *ugh* we're asked to be very small, yet we can't be smaller
        // than "MinimumViewableSize"
        if ( canMoveForward )
        {
            splitDeltaPropagate( current, splitIndex
                               , Backward, lastDirection
                               , sizeDelta );

            splitDeltaPropagate( current
                               , splitIndex + 1
                               , Forward, lastDirection
                               , - sizeDelta );
        }

        return canMoveForward;
    }

    void LayoutNode::canPropagateSplitScatter( const Screen &s
                                             , IterationDirection dir
                                             , SplitSide side
                                             , int delta ) const
    {
        size_t subId = ( dir == Forward ) ? 0 : MaxNodeCount;

        for ( size_t i = 0; i < nodes.size(); i++ )
        {
            if (nodes[i].subTree)
            {
                nodes[i].subTree->canPropagateSplit( s, subId, dir, side, delta );
            }
        }
    }

    bool LayoutLeaf::canPropagateSplit( const Screen&, int, IterationDirection
                                      , SplitSide, int ) const
        { return true; }

    bool LayoutNode::canPropagateSplit( const Screen&       s
                                      , int                 splitIndex
                                      , IterationDirection  dir
                                      , SplitSide           side
                                      , int                 delta
                                      ) const
    {
        if ( splitIndex == MaxNodeCount )
            splitIndex = nodes.size() - 1;

        if ( splitIndex < 0 || splitIndex >= int(nodes.size()) )
            return false;

        if ( side != lastDirection )
        {
            canPropagateSplitScatter(s, dir, side, delta);
            return true;
        }
        const SizePair    &previous = nodes[splitIndex];
        

        int         dim;
        
        if (lastDirection == SplitVertical)
            dim = previous.width;
        else
            dim = previous.height;

        size_t subId = ( dir == Forward ) ? 0 : MaxNodeCount;

        // we cannot satisfy the constraint.
        if ( dim == previous.nodeIncompresibleSize && delta < 0 )
        {
            previous.resizeAction = Propagate;
            return canPropagateSplit( s, splitIndex + int(dir), dir, side, delta );
        }
        else if ( dim + delta < previous.nodeIncompresibleSize )
        {
            // we can safely assume that delta is negative. If it was positive
            // we couldn't be smaller than the minimumSize.
            int newProp = delta + (dim - previous.nodeIncompresibleSize);
            previous.resizeAction = PartialPropagation;

            bool possibleMove = canPropagateSplit( s, splitIndex + int(dir)
                                                 , dir, side, newProp );
            // call our children
            if ( possibleMove && previous.subTree )
                previous.subTree->canPropagateSplit( s, subId, dir, side, newProp );

            // we can try to take the maximum of delta and forward the rest
            // to others.
            return possibleMove;
        }
        else
        {
            previous.resizeAction = UpdateInPlace;
            // recurse to know what to do on next pass
            if ( previous.subTree )
                previous.subTree->canPropagateSplit( s, subId, dir, side, delta );

            return true;
        }
    }

    void LayoutLeaf::splitDeltaPropagate( const Screen&, int, IterationDirection, SplitSide, int) {}
    void LayoutNode::splitDeltaPropagate( const Screen &s
                                        , int splitIndex
                                        , IterationDirection dir
                                        , SplitSide           side
                                        , int           delta
                                        )
    {
        if ( splitIndex == MaxNodeCount )
            splitIndex = nodes.size() - 1;

        if ( splitIndex < 0 || splitIndex >= int(nodes.size()) )
            return;

        SizePair    &previous = nodes[splitIndex];

        size_t      subId = dir == Forward ? 0 : MaxNodeCount;

        if ( side != lastDirection )
        {
            for (size_t i = 0; i < nodes.size(); i++)
            {
                if ( nodes[i].subTree )
                    nodes[i].subTree->splitDeltaPropagate(s, subId, dir, side, delta);
            }
            return;
        }

        if ( previous.resizeAction == Propagate )
            splitDeltaPropagate( s, splitIndex + int(dir), dir, side, delta );
        else if ( previous.resizeAction == PartialPropagation )
        {
            int newProp;
            
            if ( lastDirection == SplitVertical )
            {
                newProp = delta
                        + ( previous.lastLogicalDimension.width
                          - previous.nodeIncompresibleSize);
                previous.width = previous.nodeIncompresibleSize;
            }
            else
            {
                newProp = delta
                        + ( previous.lastLogicalDimension.height
                          - previous.nodeIncompresibleSize);
                previous.height = previous.nodeIncompresibleSize;
            }

            // subPropagate
            if ( nodes[ splitIndex ].subTree )
                nodes[ splitIndex ].subTree->splitDeltaPropagate( s, subId, dir, side, - newProp );

            splitDeltaPropagate( s, splitIndex + int(dir), dir, side, newProp );
        }
        // otherwise, we update
        else
        {
            // propagateSub
            if ( lastDirection == SplitVertical )
                previous.width += delta;
            else
                previous.height += delta;

            if ( nodes[splitIndex].subTree )
                nodes[splitIndex].subTree->splitDeltaPropagate( s, subId, dir, side, delta );
        }
    }

    Dimension LayoutNode::computMinimumSize() const
    {
        Dimension sub;
        Dimension rez( 0, 0 );

        for ( size_t i = 0; i < nodes.size(); i++ )
        {
            if ( nodes[i].subTree )
            {
                sub = nodes[i].subTree->computMinimumSize();
                if ( lastDirection == SplitVertical )
                {
                    rez.width += sub.width;
                    rez.height = std::max( rez.height, sub.height );
                    nodes[i].nodeIncompresibleSize = sub.width;
                }
                else
                {
                    rez.height += sub.height;
                    rez.width = std::max( rez.width, sub.width );
                    nodes[i].nodeIncompresibleSize = sub.height;
                }
            }
            else if ( lastDirection == SplitVertical )
            {
                rez.width += MinimumViewableSize;
                nodes[i].nodeIncompresibleSize = MinimumViewableSize;
            }
            else
            {
                rez.height += MinimumViewableSize;
                nodes[i].nodeIncompresibleSize = MinimumViewableSize;
            }
        }
        return rez;
    }

    Dimension LayoutLeaf::computMinimumSize() const
    {
        return Dimension( LayoutNode::MinimumViewableSize
                        , LayoutNode::MinimumViewableSize );
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

    int LayoutNode::moveSelection( IterationDirection by )
    {
        INV_CHECK;
        int count = 1;
        int newIndex = int( selectedRoute ) + int(by);

        while ( newIndex >= 0 && newIndex < int(nodes.size()))
        {
            if ( nodes[newIndex].subTree != 0 )
            {
                selectedRoute = int( newIndex );
                INV_CHECK;
                return count;
            }

            count++;
            newIndex += int(by);
        }

        INV_CHECK;
        return 0;
    }

    const Rect& LayoutNode::getSelectedSize() const
        { return nodes[selectedRoute].lastLogicalDimension; }


    LayoutTree* LayoutLeaf::pickNode( IterationDirection /*dir*/
                                    , int /*xHope*/, int /*yHope*/ ) { return this; }

    bool LayoutLeaf::checkInvariant() const
        { return &window != NULL; }

    LayoutTree* LayoutNode::pickNode( IterationDirection dir
                                    , int xHope, int yHope )
    {
        switch(lastDirection)
        {
        case SplitHorizontal:
            if ( yHope < 0 )
            {
                selectedRoute = 0;
                return nodes[0].subTree->pickNode( dir, xHope, yHope );
            }

            for (size_t i = 0; i < nodes.size(); i++)
            {
                const Rect& r = nodes[i].lastLogicalDimension;
                if ( r.y + r.height > yHope )
                {
                    if ( nodes[i].subTree )
                    {
                        return nodes[i].subTree->pickNode( dir, xHope, yHope );
                    }
                    else
                    {
                        for ( int j = int(i) + int(dir)
                            ; j >= 0 && j < int(nodes.size())
                            ; j += int(dir))
                        {
                            if (nodes[j].subTree)
                            {
                                selectedRoute = j;
                                return nodes[j].subTree->pickNode( dir, xHope, yHope );
                            }
                        }
                        // else we let the last one do the job.
                    }
                }
            }
        	break;

        case SplitVertical:
            if ( xHope < 0 )
            {
                selectedRoute = 0;
                return nodes[0].subTree->pickNode( dir, xHope, yHope );
            }

            for (size_t i = 0; i < nodes.size(); i++)
            {
                const Rect& r = nodes[i].lastLogicalDimension;
                if ( r.x + r.width > xHope )
                {
                    if ( nodes[i].subTree )
                    {
                        selectedRoute = i;
                        return nodes[i].subTree->pickNode( dir, xHope, yHope );
                    }
                    else
                    {
                        for ( int j = int(i) + int(dir)
                            ; j >= 0 && j < int(nodes.size())
                            ; j += int(dir))
                        {
                            if (nodes[j].subTree)
                            {
                                selectedRoute = j;
                                return nodes[j].subTree->pickNode( dir, xHope, yHope );
                            }
                        }
                        // else we let the last one do the job.
                    }
                }
            }
        	break;
        }

        if ( nodes[ nodes.size() - 1 ].subTree )
        {
            selectedRoute = nodes.size() - 1;
            return nodes[ selectedRoute ].subTree->pickNode( dir, xHope, yHope );
        }
        else
            return 0;
    }

    bool LayoutNode::checkInvariant() const
    {
        // the selection must be in range.
        if ( selectedRoute >= nodes.size() && nodes.size() > 1 )
            return false;

        // we should
        if ( nodes.size() > 1 )
        {
            if ( nodes[selectedRoute].subTree == NULL )
                return false;
            return nodes[selectedRoute].subTree->checkInvariant();
        }
        else // we should be deleted soon. SO don't care
            return true;
    }

    void LayoutNode::rotate( int about )
    {
        std::rotate( nodes.begin(), nodes.begin() + about, nodes.end() );
        selectedRoute = (selectedRoute + nodes.size() - about)
                      % nodes.size();
        INV_CHECK;
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
