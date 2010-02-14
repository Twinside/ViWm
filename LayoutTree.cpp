#include <assert.h>
#include <algorithm>
#include "LayoutTree.h"

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

        if (it == nodes.end()) return false;
        it->subTree = replacent;

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
            root->parent = 0;
            child->releaseChildren();
            delete child;
            break;
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
    void LayoutLeaf::Establish( const Screen& /*currentScreen*/
                              , const Rect &dim
                              , SplitSide /* side */
                              , ScreenBounded  /* bounds */ )
        { window.SetSize(dim.x, dim.y, dim.width, dim.height ); }

    void LayoutNode::Establish( const Screen &currentScreen
                              , const Rect &dim
                              , SplitSide side
                              , ScreenBounded  bounds  )

    {
        Collection::const_iterator cit;
        Collection::iterator       it;

        assert( nodes.size() > 1 );

        lastDirection = side;

        // first we need do collect some size constraints.
        int unconstrainedWidth = dim.width;
        int unconstrainedHeight = dim.height;
        int unconstrainedWidthCount = 0;
        int unconstrainedHeightCount = 0;

        for (cit = nodes.begin(); cit != nodes.end(); ++cit )
        {
            if (cit->width > 0)
            {
                unconstrainedWidth -= cit->width;
                unconstrainedWidthCount++;
            }

            if (cit->height > 0)
            {
                unconstrainedHeight -= cit->height;
                unconstrainedHeightCount++;
            }
        }

        Rect        subSize;
        subSize = dim;

        // during the second pass, we give real sizes to nodes
        if ( side == SplitHorizontal )
        {
            int unconstrainedSize = unconstrainedHeightCount > 0
                ? unconstrainedHeight / unconstrainedHeightCount
                : dim.height / static_cast<int>( nodes.size() );

            size_t i = 0;
            for (it = nodes.begin(); it != nodes.end(); ++it, i++)
            {
                int subBound = bounds;
                int sizeSub = 0;
                int topShift = 0;

                if (i != 0 || (/*i == 0 &&*/ (bounds & TopBound) == 0))
                {
                    subBound &= ~TopBound;
                    sizeSub += HalfSplit;
                    topShift += HalfSplit;
                }

                if (i != nodes.size() - 1 ||
                    (/*i == nodes.size() - 1 &&*/ (bounds & BottomBound) == 0))
                {
                    subBound &= ~BottomBound;
                    sizeSub += HalfSplit;
                }

                if ( it->height )
                    subSize.height = it->height - sizeSub;
                else
                    subSize.height = unconstrainedSize - sizeSub;

                subSize.y += topShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitVertical
                                          , ScreenBounded(subBound) );
                }
                subSize.y += subSize.height - topShift + sizeSub;
                subSize.height += sizeSub;

                it->lastDim = subSize;
            }
        }
        else // SplitVertical
        {
            int unconstrainedSize = unconstrainedWidthCount > 0
                ? unconstrainedWidth / unconstrainedWidthCount
                : dim.width / static_cast<int>( nodes.size() );

            size_t i = 0;

            for (it = nodes.begin(); it != nodes.end(); ++it, i++)
            {
                int subBound = bounds;
                int sizeSub = 0;
                int leftShift = 0;

                // if we're not at the left (first index), OR
                // we're the first, but hey, we're not at the left
                // of screen :(
                if (i != 0 || (/*i == 0 && */(bounds & LeftBound) == 0))
                {
                    subBound &= ~LeftBound; // We're not at left
                    sizeSub += HalfSplit;   // we're smaller
                    leftShift += HalfSplit; // we must shift beginning.
                }

                if (i != nodes.size() - 1 ||
                    (/*i == nodes.size() - 1 && */(bounds & RightBound) == 0))
                {
                    subBound &= ~RightBound;
                    sizeSub += HalfSplit;
                }

                if ( it->width )
                    subSize.width = it->width - sizeSub;
                else
                    subSize.width = unconstrainedSize - sizeSub;

                subSize.x += leftShift;
                if ( it->subTree )
                {
                    it->subTree->Establish( currentScreen
                                          , subSize
                                          , SplitVertical
                                          , ScreenBounded(subBound) );
                }
                subSize.x += subSize.width - leftShift + sizeSub;
                subSize.width += sizeSub;
                it->lastDim = subSize;
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
            || nodes[selectedRoute].subTree->addNode( window ) == Todo )
            nodes.push_back( window );

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
    LayoutTree::CompStatus LayoutLeaf::removeNode( LayoutTree* /* toRemove */ ) { return Done; }
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
            pack( nodes[i].subTree->removeNode( toRemove ), i );

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
    ////                        Layout drawing
    //////////////////////////////////////////////////////////////////////////
    void LayoutLeaf::displayLayoutStructure( Renderer& /*r*/, Renderer::Brush /* defaultBrush */ ) {}
    void LayoutNode::displayLayoutStructure( Renderer &r, Renderer::Brush defaultBrush )
    {
        Collection::iterator it;

        if ( lastDirection == SplitHorizontal )
        {
            it = nodes.begin();
            r.drawRect( defaultBrush
                      , it->lastDim.x
                      , it->lastDim.y
                      , it->lastDim.width - HalfSplit
                      , SplitWidth );

            for (it = nodes.begin() + 1; it != nodes.end() - 1; ++it)
            {
                r.drawRect( defaultBrush
                          , it->lastDim.x
                          , it->lastDim.y
                          , it->lastDim.width - SplitWidth
                          , SplitWidth );
            }
        }
        else
        {
            for (it = nodes.begin(); it != nodes.end(); ++it)
                r.drawRect( defaultBrush
                          , it->lastDim.x
                          , it->lastDim.y
                          , SplitWidth
                          , it->lastDim.height );
        }
    }
}
