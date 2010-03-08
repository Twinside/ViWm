#ifndef LAYOUTTREE_H
#define LAYOUTTREE_H

#include <algorithm>
#include <vector>
#include <functional>
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

    class LayoutNode;

    /**
     * This struct only exist to provide a type
     * token. the tree structure is really a sum
     * type.
     */
    struct LayoutTree
    {
        LayoutTree();
        virtual ~LayoutTree();

        /**
         * Describe how the space will be split.
         */
        enum SplitSide
        {
            // +--------------------------------------------------+
            // |                                                  |
            // |           Window 1 (or an imbricated split)      |
            // |                                                  |
            // +--------------------------------------------------+
            // +--------------------------------------------------+
            // |                                                  |
            // |          Window 2                                |
            // |                                                  |
            // |                                                  |
            // +--------------------------------------------------+
            // +--------------------------------------------------+
            // |                                                  |
            // |          Window 3                                |
            // |                                                  |
            // +--------------------------------------------------+
            SplitHorizontal,

            // +-------------+-+----------------+-+---------------+
            // |             | |                | |               |
            // |  Window 1   | |  Window2       | |  Window3      |
            // |             | |  (or an        | |               |
            // |             | |   imbricated   | |               |
            // |             | |      split)    | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // |             | |                | |               |
            // +-------------+-+----------------+-+---------------+
            SplitVertical
        };

        /**
         * Used to refer to a system window.
         */
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

        /**
         * Force the subnode/contained window to fit
         * the current tree dimension. Refresh the
         * layout simply put.
         */
        virtual void    Establish( const Screen &currentScreen
                                 , const Rect &dim
                                 , SplitSide side
                                 ) = 0;

        /**
         * Add a node to the tree. The new node is inserted
         * near the currently selected node
         */
        virtual CompStatus addNode( LayoutTree *subTree ) = 0;

        /**
         * Remove the leaf with the given window if it exists.
         */
        virtual CompStatus removeNode( WindowKey toRemove ) = 0;

        /**
         * Remove ALL LayoutTree that has the same adress. It
         * means you can cleanup all the empty cell by calling
         * it with 0.
         */
        virtual CompStatus removeNode( LayoutTree *toRemove ) = 0;

        /**
         * Search the tree for a node, and give it the focus.
         */
        virtual CompStatus selectNode( WindowKey toSelect ) = 0;

        /**
         * Get the currently selected leaf.
         */
        virtual LayoutTree* getSelected() = 0;

        typedef std::pair<LayoutNode*, size_t> SplitCoord;
        virtual SplitCoord FindPointedSplit( int x, int y ) = 0;

        /**
         * Given a tree root, update the root to add the new node.
         * The root can be null. If root exist, the tree will be
         * appended.
         */
        static CompStatus addCreate( LayoutTree *&root, LayoutTree &tree );

        /**
         * Remove a window, and if there is no need for a tree anymore,
         * reset root to null.
         */
        static CompStatus removeClean( LayoutTree *&root, WindowKey key );
        static CompStatus removeClean( LayoutTree *&root, LayoutTree *tree );

        LayoutTree  *parent;

        /**
         * Draw the splits given a window and a good brush.
         * Use the information computed in the Establish
         * method. And thus, must be called after Establish.
         */
        virtual void        displayLayoutStructure
                            ( Renderer::RenderWindow &r
                            , Renderer::RenderWindow::Brush defaultBrush ) const = 0;

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
             * then the width is free of constraint.
             * Constraint is exrpimed in ration of screen
             * size.
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

        typedef std::tr1::function<bool (SizePair&)>   IteratingPredicate;

        bool FocusTopIteration( IteratingPredicate &p );
        bool DepthFirstIteration( IteratingPredicate &p );
        bool Iter( IteratingPredicate &p );
        size_t  getSubNodeCount() const { return nodes.size(); }

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
        virtual SplitCoord    FindPointedSplit( int x, int y );

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

        /** 
         * Physically move a virtual split on screen.
         * Try to enforce some constraint to get a coherent screen.
         * @param current Screen containing the layout.
         * @param x horizontal position in screen coordinate
         * @param y vertical position in screen coordinate
         * @param splitIndex ID of the selected index
         * @return true if the split has been moved, false otherwise.
         */
        bool moveSplit( const Screen &current, int x, int y, size_t splitIndex );

        Rect        getMyPreviousDimension( const Screen &current ) const;

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
            HalfSplit = SplitWidth / 2,
            MinimumViewableSize = SplitWidth * 2
        };

        enum IterationDirection
        {
            Forward = 1,
            Backward = -1
        };

        bool        splitDeltaPropagate( const Screen &s
                                       , size_t splitIndex
                                       , IterationDirection dir
                                       , int delta
                                       );

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
        virtual SplitCoord    FindPointedSplit( int x, int y );

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
