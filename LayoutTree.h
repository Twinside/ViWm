#ifndef LAYOUTTREE_H
#define LAYOUTTREE_H

#include <vector>
#include "TilledWindow.h"

/**
 * This struct only exist to provide a type
 * token. the tree structure is really a sum
 * type.
 */
struct LayoutTree
{
    virtual ~LayoutTree();

    enum SplitSide
    {
        SplitHorizontal,
        SplitVertical
    };

    /**
     * Force the subnode/contained window to fit
     * the current tree dimension. Refresh the
     * layout simply put.
     */
    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height ) = 0;
};

struct LayoutNode : LayoutTree
{
    virtual ~LayoutNode();

    enum Constraint
    {
        Unconstrained = -1
    };

    struct SizePair
    {
        int         width;
        int         height;
        LayoutTree* subTree;
    };

    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height );
    typedef std::vector<SizePair> Collection;

    Collection  nodes;
};

struct LayoutLeaf : LayoutTree
{
    LayoutLeaf( TilledWindow &w ) : window( w ) {}

    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height );

    TilledWindow   &window;
};

#endif /* LAYOUTTREE_H */
