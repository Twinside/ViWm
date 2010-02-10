#include "LayoutTree.h"

LayoutTree::~LayoutTree() {}

LayoutTree::CompStatus LayoutTree::addCreate( LayoutTree *&root, LayoutTree &tree )
{
    if ( root == 0 )
        root = &tree;
    else if (root->addNode( tree ) == Todo)
    {
        LayoutTree  *prevRoot = root;
        root = new LayoutNode();
        root->addNode( *prevRoot );
        root->addNode( tree );
    }

    return Done;
}

LayoutTree::CompStatus LayoutTree::removeClean( LayoutTree *&root, WindowKey key )
{
    if ( root != 0 && root->removeNode( key ) == Todo )
    {
        delete root;
        root = 0;
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
void LayoutLeaf::Establish( SplitSide /*side*/ , int x, int y , int width, int height )
    { window.SetSize(x, y, width, height ); }

void LayoutNode::Establish( SplitSide   side, int x, int y , int width, int height )
{
    Collection::const_iterator cit;
    Collection::iterator       it;

    // first we need do collect some size constraints.
    int unconstrainedWidth = width;
    int unconstrainedHeight = height;
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

    // during the second pass, we give real sizes to nodes
    if ( side == SplitHorizontal )
    {
        int unconstrainedSize = unconstrainedHeightCount > 0
                              ? unconstrainedHeight / unconstrainedHeightCount
                              : height / nodes.size();

        for (it = nodes.begin(); it != nodes.end(); ++it)
        {
            if ( it->height )
            {
                it->subTree->Establish( SplitVertical, x, y, width, it->height );
                y += it->height;
            }
            else
            {
                it->subTree->Establish( SplitVertical, x, y, width, unconstrainedSize );
                y += unconstrainedSize;
            }
        }
    }
    else // SplitVertical
    {
        int unconstrainedSize = unconstrainedWidthCount > 0
                              ? unconstrainedWidth / unconstrainedWidthCount
                              : width / nodes.size();

        for (it = nodes.begin(); it != nodes.end(); ++it)
        {
            if ( it->width )
            {
                it->subTree->Establish( SplitVertical, x, y, it->width, height );
                x += it->height;
            }
            else
            {
                it->subTree->Establish( SplitVertical, x, y, unconstrainedSize, height );
                x += unconstrainedSize;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
////                    Node adding
//////////////////////////////////////////////////////////////////////////

// idea of the algorithm, we go down to the the selected node
// when we found the selected leaf, we backtrack once and add
// the node there.
LayoutTree::CompStatus LayoutNode::addNode( LayoutTree &window )
{
    if ( nodes.size() == 0
      || nodes[selectedRoute].subTree->addNode( window ) == Todo )
        nodes.push_back( &window );

    return Done;
}

LayoutTree::CompStatus LayoutLeaf::addNode( LayoutTree& /*window*/ )
    { return Todo; }

//////////////////////////////////////////////////////////////////////////
////                    Node Removing
//////////////////////////////////////////////////////////////////////////

// Depth First Search + same idea of backtracking as in node adding.
LayoutTree::CompStatus LayoutLeaf::removeNode( WindowKey toRemove )
    { return window == toRemove ? Todo : Searching; }

LayoutTree::CompStatus LayoutNode::removeNode( WindowKey toRemove )
{
    Collection::iterator    it;

    for ( it = nodes.begin(); it != nodes.end(); ++it )
    {
        switch ( it->subTree->removeNode( toRemove ) )
        {
        case Todo:
            nodes.erase( it );

            if ( nodes.size() == 0 )
                return Todo;

            return Done;

        case Done: return Done;

        case Searching:
            /* Continue to search */
            break;
        }
    }

    return Searching;
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
        if ( nodes[i].subTree->selectNode( toSelect ) != Searching )
        {
            selectedRoute = i;
            return Done;
        }
    }

    return Searching;
}
