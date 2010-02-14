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
        Screen( LayoutTree::WindowKey associated
              , int x, int y, int width, int height );

        // This accessor are a bit ugly, but I don't
        // want some change screen size by mistake.
        int getX()      { return size.x; }
        int getY()      { return size.y; }
        int getWidth()  { return size.width; }
        int getHeight() { return size.height; }

        void    replace()
        { 
            if (layoutRoot)
                layoutRoot->Establish( *this, size, initialSplit
                                     , LayoutTree::FullBound );
        }

        LayoutTree::SplitSide   initialSplit;
        LayoutTree              *layoutRoot;

    private:
        Rect    size;
        LayoutTree::WindowKey fullScreenWin;
    };

    typedef std::vector< Screen >   DesktopLayout;
}

#endif /* SCREEN_H */
