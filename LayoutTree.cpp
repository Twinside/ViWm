#include "LayoutTree.h"

LayoutTree::~LayoutTree() {}

LayoutNode::~LayoutNode()
{
    for ( Collection::iterator it = nodes.begin()
        ; it != nodes.end()
        ; ++it )
    {
        delete it->subTree;
    }
}

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

    int tempSplitDimension;

    // during the second pass, we give real sizes to nodes
    if ( side == SplitHorizontal )
    {
        int unconstrainedSize = unconstrainedHeight / unconstrainedHeightCount;

        for (it = nodes.begin(); cit != nodes.end(); ++it)
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
        int unconstrainedSize = unconstrainedWidth / unconstrainedWidthCount;

        for (it = nodes.begin(); cit != nodes.end(); ++it)
        {
            if ( it->width )
            {
                it->subTree->Establish( SplitVertical, x, y, it->width, height );
                y += it->height;
            }
            else
            {
                it->subTree->Establish( SplitVertical, x, y, unconstrainedSize, height );
                y += unconstrainedSize;
            }
        }
    }
}

void LayoutLeaf::Establish( SplitSide side , int x, int y , int width, int height )
    { window.SetSize(x, y, width, height ); }
