#include "Screen.h"

namespace ViWm
{
    Screen::Screen( Renderer::RenderWindow &associated
                  , int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        , fullScreenWin( associated )
        , initialSplit( LayoutTree::SplitHorizontal )
       // , initialSplit( LayoutTree::SplitVertical )
        , splitBrush( associated.CreateBrush( 0.5f, 0.5f, 0.7f, 1.0f ) )
    {}
    
    Screen::~Screen()
    {
        fullScreenWin.DeleteBrush( splitBrush );
    }

    void Screen::replace()
    {
        if (!layoutRoot) return;

        layoutRoot->Establish( *this, size, initialSplit
                             , LayoutTree::FullBound );
        layoutRoot->DisplaySplitTree( fullScreenWin, splitBrush );
    }
}
