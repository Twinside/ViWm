#ifndef __TESTTREEBUILDER_H__
#define __TESTTREEBUILDER_H__

#include "../LayoutTree.h"

namespace ViWm
{
    /**
     * This class is used to build tree for tests
     */
    class TreeTestBuilder
    {
    public:
       inline static void addNode( LayoutNode &node, LayoutTree *toAdd )
           { node.nodes.push_back( LayoutNode::SizePair( toAdd ) ); }

       inline static void setSelection( LayoutNode &node, size_t select )
           { node.selectedRoute = select; }
    private:
    };
}

#endif /* __TESTTREEBUILDER_H__ */
