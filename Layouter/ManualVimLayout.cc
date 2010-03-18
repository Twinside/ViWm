#include "ManualVimLayout.h"
#include "../State.h"
#include "../Screen.h"

namespace ViWm {
namespace Layout
{
    ManualVimLayout::ManualVimLayout( const Configuration &c )
        : conf( c )
    {}

    void ManualVimLayout::layout( const WindowMakerState &s, DesktopLayout &l )
    {
        Screen  &cs = l[ s.currentScreen ];

        if ( !cs.layoutRoot )
            return;

        LayoutTree *tree = cs.layoutRoot->getSelected();

        if ( !tree )
            return;

        normalizeNode( cs
                     , static_cast<LayoutLeaf*>( tree )
                     , tree->getParent() );
    }


    void ManualVimLayout::reLayout( const Screen &s, LayoutNode &n )
        { normalizeNode( s, 0, &n ); }

    class EmptyFinder
    {
    public:
        EmptyFinder( TilledWindow& n ) : what( n ) {}

        bool operator() ( LayoutNode&   father, LayoutNode::SizePair &p )
        {
            if ( p.subTree == 0 )
            {
                p.subTree = new LayoutLeaf( what );
                p.subTree->parent = &father;
                return true;
            }

            return false;
        }

    private:
        TilledWindow&  what;
        void operator =(const EmptyFinder&) const {}
    };

    void ManualVimLayout::addNewWindowToLayout( TilledWindow &newWindow
                                              , const WindowMakerState &st
                                              , DesktopLayout &l )
    {
        // first we try to find an open view in the tree
        EmptyFinder finder( newWindow );

        LayoutNode::IteratingPredicate  pred( finder );

        LayoutNode* node = dynamic_cast<LayoutNode*>(l[ st.currentScreen ].layoutRoot);

        // try to find the nearest empty space to insert ourselves
        if ( node && node->FocusTopIteration( pred ) )
            return;

        LayoutLeaf  *leaf = new LayoutLeaf( newWindow );

        // if we don't find one, we digg to find a new one :)
        LayoutTree::addCreate( l[ st.currentScreen ].layoutRoot
                             , *leaf );

        normalizeNode( l[ st.currentScreen ]
                     , leaf
                     , leaf->getParent() );
    }

    // Struct outside normalizeNode only because G++
    // refuse to compile it nicely.
    struct SizeSetter
    {
        int widthAdd, widthRest;
        int heightAdd, heightRest;
        int i;

        SizeSetter( int w, int wrest, int h, int hrest )
            : widthAdd( w ), widthRest( wrest )
            , heightAdd( h ), heightRest( hrest )
            , i( 0 ) {}
        
        bool   operator() ( LayoutNode& /*doesn't need*/
                          , LayoutNode::SizePair &p )
        {
            p.width = widthAdd;
            p.height = heightAdd;

            if ( i++ == 0 )
            {
                p.width += widthRest;
                p.height += heightRest;
            }
            return false;
        }
    };

    struct SizeSplitter
    {
        LayoutNode::SizePair    *prev;
        LayoutLeaf              *leaf;
        SizeSplitter( LayoutLeaf *l ) : prev( 0 ), leaf(l) {}

        bool   operator() ( LayoutNode& /*doesn't need*/, LayoutNode::SizePair &p )
        {
            prev = &p;
            if ( p.subTree == leaf )
            {
                p.width = prev->width / 2;
                p.height = prev->height / 2;
                prev->width = prev->width / 2 + prev->width % 2;
                prev->height = prev->height / 2 + prev->height % 2;
            }
            return false;
        }
    };

    void ManualVimLayout::normalizeNode( const Screen &s, LayoutLeaf *leaf, LayoutNode* node )
    {
        if (!node) return;

        // please use lambda when updating compiler.
        int     nodeCount = int(node->getSubNodeCount());
        Rect    size = node->getMyPreviousDimension( s );

        if (conf.getEqualAlways())
        {
            SizeSetter  setter( size.width / nodeCount
                              , size.width % nodeCount
                              , size.height / nodeCount
                              , size.height % nodeCount );
            LayoutNode::IteratingPredicate functor( setter );
            node->Iter( functor );
        }
        else
        {
            SizeSplitter    splitter( leaf );
            LayoutNode::IteratingPredicate  functor( splitter );
            node->Iter( functor );
        }
    }
}}
