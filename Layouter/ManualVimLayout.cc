#include "ManualVimLayout.h"
#include "../State.h"
#include "../Screen.h"

namespace ViWm {
namespace Layout
{
    void ManualVimLayout::layout( const WindowMakerState &s, DesktopLayout &l )
    { 
        /* This Layout does nothing by himself
         *
         */
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

        // if we don't find one, we digg to find a new one :)
        LayoutTree::addCreate( l[ st.currentScreen ].layoutRoot
                             , *new LayoutLeaf( newWindow ) );
    }
}}
