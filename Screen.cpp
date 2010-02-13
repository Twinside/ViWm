#include "Screen.h"

namespace ViWm
{
    Screen::Screen()
        : layoutRoot( 0 )
        , initialSplit( LayoutTree::SplitHorizontal )
    {}

    Screen::Screen(int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        //, initialSplit( LayoutTree::SplitHorizontal )
        , initialSplit( LayoutTree::SplitVertical )
    {}
}