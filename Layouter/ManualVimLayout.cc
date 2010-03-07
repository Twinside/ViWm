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
        // need to check constraint consistency.
    }

    void ManualVimLayout::addNewWindowToLayout( TilledWindow &newWindow
                                              , const WindowMakerState &st
                                              , DesktopLayout &l )
    {
        // first we try to find an open view in the tree
        struct EmptyFinder
        {
            TilledWindow&  what;
            EmptyFinder( TilledWindow& n ) : what( n ) {}
            

            bool operator() ( LayoutNode::SizePair &p )
            {
                if ( p.subTree == 0 )
                {
                    p.subTree = new LayoutLeaf( what );
                    return true;
                }

                return false;
            }

        private:
            void operator =(const EmptyFinder&) {}
        } finder( newWindow );

        LayoutNode::IteratingPredicate  pred( finder );

        LayoutNode* node = dynamic_cast<LayoutNode*>(l[ st.currentScreen ].layoutRoot);

        // try to find the nearest empty space to insert ourselves
        if ( node && node->FocusTopIteration( pred ) )
            return;

        LayoutLeaf  *leaf = new LayoutLeaf( newWindow );

        // if we don't find one, we digg to find a new one :)
        LayoutTree::addCreate( l[ st.currentScreen ].layoutRoot
                             , *leaf );

        normalizeNode( leaf, static_cast<LayoutNode*>(leaf->parent) );
    }

    void ManualVimLayout::normalizeNode( LayoutLeaf *leaf, LayoutNode* node )
    {
        if (!node) return;

        // please use lambda when updating compiler.
        struct SizeSetter
        {
            float  amount;
            SizeSetter( float namount ) : amount( namount ) {}
            
            bool   operator() ( LayoutNode::SizePair &p )
            {
                p.height = amount;
                p.width = amount;
                return false;
            }
        };

        struct SizeSplitter
        {
            LayoutNode::SizePair    *prev;
            LayoutLeaf              *leaf;
            SizeSplitter( LayoutLeaf *l ) : prev( 0 ), leaf(l) {}

            bool   operator() ( LayoutNode::SizePair &p )
            {
                prev = &p;
                if ( p.subTree == leaf )
                {
                    p.width = prev->width / 2.0f;
                    p.height = prev->height / 2.0f;
                    prev->width /= 2.0f;
                    prev->height /= 2.0f;
                }
                return false;
            }
        };

        size_t  nodeCount = node->getSubNodeCount();

        if (conf.getEqualAlways())
        {
            SizeSetter  setter( 1.0f / float(nodeCount) );
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
