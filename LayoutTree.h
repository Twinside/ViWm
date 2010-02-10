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

    typedef HWND    WindowKey;

    enum CompStatus
    {
        Searching,  /**< Still need to search. */
        Todo,       /**< The action must be performed. */
        Done        /**< The action has been performed. */
    };

    virtual CompStatus addNode( LayoutTree &subTree ) = 0;
    virtual CompStatus removeNode( WindowKey toRemove ) = 0;
    virtual CompStatus selectNode( WindowKey toRemove ) = 0;

    static CompStatus addCreate( LayoutTree *&root, LayoutTree &tree );
    static CompStatus removeClean( LayoutTree *&root, WindowKey key );

    /**
     * Force the subnode/contained window to fit
     * the current tree dimension. Refresh the
     * layout simply put.
     */
    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height ) = 0;
};

struct LayoutNode : public LayoutTree
{
    LayoutNode();
    virtual ~LayoutNode();

    enum Constraint
    {
        Unconstrained = -1
    };

    struct SizePair
    {
        SizePair( LayoutTree *t )
            : width( 0 )
            , height( 0 )
            , subTree( t ) {}

        int         width;
        int         height;
        LayoutTree* subTree;
    };

    virtual CompStatus    addNode( LayoutTree &subTree );
    virtual CompStatus    removeNode( WindowKey toRemove );
    virtual CompStatus    selectNode( WindowKey toSelect );

    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height
                             );

    typedef std::vector<SizePair> Collection;

    Collection  nodes;
    size_t      selectedRoute;
};

class LayoutLeaf : public LayoutTree
{
public:
    LayoutLeaf( TilledWindow &w ) : window( w ) {}

    virtual CompStatus    addNode( LayoutTree &window );
    virtual CompStatus    removeNode( WindowKey toRemove );
    virtual CompStatus    selectNode( WindowKey toSelect );

    virtual void    Establish( SplitSide side
                             , int x, int y
                             , int width, int height );

    TilledWindow   &window;

private:
    void operator = ( const LayoutLeaf& l );
};

class WindowMakerState;


#endif /* LAYOUTTREE_H */
