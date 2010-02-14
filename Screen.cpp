#include "Screen.h"

namespace ViWm
{
    Screen::Screen( LayoutTree::WindowKey associated
                  , int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        , fullScreenWin( associated )
        , initialSplit( LayoutTree::SplitHorizontal )
       // , initialSplit( LayoutTree::SplitVertical )
    {}
}