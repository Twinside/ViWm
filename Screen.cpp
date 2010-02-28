#include "Screen.h"

namespace ViWm
{
    Screen::Screen( Renderer::RenderWindow &associated
                  , int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        , fullScreenWin( &associated )
        , initialSplit( LayoutTree::SplitHorizontal )
       // , initialSplit( LayoutTree::SplitVertical )
        , splitBrush( associated.CreateBrush( 10, 10, 30, 255 ) )
    {}
    
    Screen::~Screen()
    {
        fullScreenWin->DeleteBrush( splitBrush );
    }

    void Screen::replace()
    {
        if (!layoutRoot) return;

        layoutRoot->Establish( *this, size, initialSplit );
        layoutRoot->DisplaySplitTree( *fullScreenWin, splitBrush );
    }
}
