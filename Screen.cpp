#include "Screen.h"

namespace ViWm
{
    Screen::Screen()
        : layoutRoot( 0 )
        , x( 0 ), y( 0 )
        , width( 0 ), height( 0 )
        , initialSplit( LayoutTree::SplitHorizontal )
    {}

    Screen::Screen(int nx, int ny, int nwidth, int nheight)
        : x( nx )
        , y( ny )
        , width( nwidth )
        , height( nheight )
        , layoutRoot( 0 )
        //, initialSplit( LayoutTree::SplitHorizontal )
        , initialSplit( LayoutTree::SplitVertical )
    {}
}