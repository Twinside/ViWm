#ifndef LAYOUTTREE_H
#define LAYOUTTREE_H

#include <algorithm>
#include <vector>
#include "TilledWindow.h"
#include "Rendering/RenderWindow.h"

namespace ViWm
{
    struct Screen;

    struct Rect
    {
        Rect() : x(0), y(0), width(0), height(0) {}
        Rect( int xx, int yy, int w, int h )
            : x( xx ), y( yy ), width( w ), height( h ) {}

        int x, y;
        int width, height;
    };

    /**
     * This struct only exist to provide a type
     * token. the tree structure is really a sum
     * type.
     */
    struct LayoutTree
    {
        LayoutTree();
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
            /**
             * Give the order to replace the node
             * with it's first child
             */
            Compact,    
            Done        /**< The action has been performed. */
        };

        void   DisplaySplitTree( Renderer::RenderWindow &r, Renderer::RenderWindow::Brush &defaultBrush ) const;

        virtual CompStatus addNode( LayoutTree *subTree ) = 0;
        virtual CompStatus removeNode( WindowKey toRemove ) = 0;
        virtual CompStatus removeNode( LayoutTree *toRemove ) = 0;
        virtual CompStatus selectNode( WindowKey toRemove ) = 0;
        virtual LayoutTree* getSelected() = 0;

        static CompStatus addCreate( LayoutTree *&root, LayoutTree &tree );
        static CompStatus removeClean( LayoutTree *&root, WindowKey key );
        static CompStatus removeClean( LayoutTree *&root, LayoutTree *tree );

        /**
         * Force the subnode/contained window to fit
         * the current tree dimension. Refresh the
         * layout simply put.
         */
        virtual void    Establish( const Screen &currentScreen
                                 , const Rect &dim
                                 , SplitSide side
                                 ) = 0;

        LayoutTree  *parent;

    protected:
        virtual void        displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::RenderWindow::Brush defaultBrush ) const = 0;

        friend class LayoutNode;

    private:
        static CompStatus   globalPack( LayoutTree *&root, CompStatus st );
    };

    /**
     * Subdivision node. Split the screen in a
     * direction (horizontal or vertical). It
     * can has empty nodes (for a future opened
     * node).
     */
    class LayoutNode : public LayoutTree
    {
    public:
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

            /**
             * Constraining weight. If width == 0,
             * then the width is free of constraint
             */
            int         width;
            int         height; /**< Same thing as width */

            /**
             * Store the width atributed to the subtree
             * during the last establishment
             */
            Rect        lastDim;
            LayoutTree* subTree;
        };

        SplitSide   getLastDirection() { return lastDirection; }

        /**
         * Return the first subnode of this node.
         */
        LayoutTree* getFirstNode();

        /**
         * For all the subtree in nodes, set the pointer to 0.
         * Do not deallocate them.
         */
        void        releaseChildren();

        virtual CompStatus    addNode( LayoutTree *subTree );
        virtual CompStatus    removeNode( WindowKey toRemove );
        virtual CompStatus    removeNode( LayoutTree *toRemove );
        virtual CompStatus    selectNode( WindowKey toSelect );
        virtual LayoutTree*   getSelected();

        virtual void    Establish( const Screen &currentScreen
                                 , const Rect &dim
                                 , SplitSide side );

        typedef std::vector<SizePair> Collection;

        // this node only.
        bool    replace( LayoutTree *toSearch, LayoutTree *replacent );

        inline void    insertAfter( LayoutTree *toSearch, LayoutTree *toAdd )
            { insert( toSearch, toAdd, 1 ); }

        inline void    insertBefore( LayoutTree* toSearch, LayoutTree *toAdd )
            { insert( toSearch, toAdd, 0 ); }

        inline void    rotate( int about )
            { std::rotate( nodes.begin(), nodes.begin() + about, nodes.end() ); }

    protected:
        virtual void        displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::RenderWindow::Brush defaultBrush ) const;

    private:
        CompStatus  pack( CompStatus what, size_t &index );
        void        insert( LayoutTree  *toSearch
                          , LayoutTree  *toAdd
                          , int plusMinus );


        enum    Conf
        {
            SplitWidth = 8,
            HalfSplit = SplitWidth / 2
        };

        SplitSide   lastDirection;
        Collection  nodes;
        size_t      selectedRoute;
    };

    class LayoutLeaf : public LayoutTree
    {
    public:
        LayoutLeaf( TilledWindow &w ) : window( w ) {}

        virtual CompStatus    addNode( LayoutTree *window );
        virtual CompStatus    removeNode( WindowKey toRemove );
        virtual CompStatus    selectNode( WindowKey toSelect );
        virtual CompStatus    removeNode( LayoutTree *toRemove );
        virtual LayoutTree*   getSelected();

        virtual void    Establish( const Screen &currentScreen
                                 , const Rect &dim
                                 , SplitSide side );

        TilledWindow   &window;

    protected:
        virtual void        displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::RenderWindow::Brush defaultBrush ) const;

    private:
        void operator = ( const LayoutLeaf& l );
    };

    class WindowMakerState;
}

#endif /* LAYOUTTREE_H */
