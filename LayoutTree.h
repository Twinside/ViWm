#ifndef LAYOUTTREE_H
#define LAYOUTTREE_H

#include <list>
#include "TilledWindow.h"

/**
 * This struct only exist to provide a type
 * token. the tree structure is really a sum
 * type.
 */
struct LayoutTree {};

struct LayoutNode : LayoutTree
{
    std::list<LayoutTree*>  nodes;
};

struct LayoutLeaf : LayoutTree
{
    TilledWindow   *w;
};


#endif /* LAYOUTTREE_H */