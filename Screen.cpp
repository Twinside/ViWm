#include "Screen.h"

namespace ViWm
{
    Screen::Screen( Renderer::RenderWindow &associated
                  , int nx, int ny, int nwidth, int nheight)
        : size( nx, ny, nwidth, nheight)
        , layoutRoot( 0 )
        , fullScreenWin( &associated )
        , initialSplit( LayoutTree::SplitHorizontal )
    {}
    
    Screen::~Screen()
    {
    }

    void Screen::replace(bool transparentVoid)
    {
        if (!layoutRoot) return;

        Rect    layoutSize( 0, 0, size.width, size.height );

        layoutRoot->Establish( *this, layoutSize, initialSplit );

        Renderer::RenderWindow::Brush   splitBrush =
            fullScreenWin->CreateBrush( 30, 30, 30, 255 );

        fullScreenWin->begin( transparentVoid );
        layoutRoot->displayLayoutStructure( *fullScreenWin, splitBrush );
        fullScreenWin->end();

        fullScreenWin->DeleteBrush( splitBrush );
    }

    bool Screen::isInScreenBound(int x, int y) const
    {
    	return x >= size.x && y >= size.y
            && x < size.x + size.width
            && y < size.y + size.height;
    }

    LayoutTree::SplitCoord    Screen::FindPointedSplit( int x, int y )
    {
        if ( !layoutRoot )
            return LayoutTree::SplitCoord( NULL, 0 );

        return layoutRoot->FindPointedSplit( x - size.x, y - size.y );
    }
}
