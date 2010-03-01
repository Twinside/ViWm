#include "Screen.h"

namespace ViWm
{
    Screen::Screen( Renderer::RenderWindow &associated
                  , int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        , fullScreenWin( &associated )
        , initialSplit( LayoutTree::SplitVertical )
    {}
    
    Screen::~Screen()
    {
    }

    void Screen::replace()
    {
        if (!layoutRoot) return;

        layoutRoot->Establish( *this, size, initialSplit );

        Renderer::RenderWindow::Brush   splitBrush =
            fullScreenWin->CreateBrush( 30, 30, 30, 255 );
        layoutRoot->DisplaySplitTree( *fullScreenWin, splitBrush );

        fullScreenWin->DeleteBrush( splitBrush );
    }
}
