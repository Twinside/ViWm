#ifndef SCREEN_H
#define SCREEN_H

#include "LayoutTree.h"

namespace ViWm
{
    /**
     * Represent a display.
     * Now dual or more screen are usual for programmers,
     * we must handle it nicely.
     */
    struct Screen 
    {
        Screen();
        Screen( int x, int y, int width, int height );

        // This accessor are a bit ugly, but I don't
        // want some change screen size by mistake.
        int getX()      { return x; }
        int getY()      { return y; }
        int getWidth()  { return width; }
        int getHeight() { return height; }

        void    replace()
        { 
            if (layoutRoot)
                layoutRoot->Establish( initialSplit, x, y, width, height );
        }

        LayoutTree::SplitSide   initialSplit;
        LayoutTree              *layoutRoot;

    private:
        int x, y;
        int width, height;
    };

    typedef std::vector< Screen >   DesktopLayout;
}

#endif /* SCREEN_H */
