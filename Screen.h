#ifndef SCREEN_H
#define SCREEN_H

#include "LayoutTree.h"

/**
 * Represent a display.
 * Now dual or more screen are usual for programmers,
 * we must handle it nicely.
 */
struct Screen 
{
    Screen();
    Screen( int x, int y, int width, int height );

    int x, y;
    int width, height;
    LayoutTree  *layoutRoot;
};

typedef std::vector< Screen >   DesktopLayout;

#endif /* SCREEN_H */