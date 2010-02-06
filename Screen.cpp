#include "Screen.h"

Screen::Screen()
    : layoutRoot( 0 )
    , x( 0 ), y( 0 )
    , width( 0 ), height( 0 )
{}

Screen::Screen(int nx, int ny, int nwidth, int nheight)
    : x( nx )
    , y( ny )
    , width( nwidth )
    , height( nheight )
    , layoutRoot( 0 )
{}
