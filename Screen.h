#ifndef SCREEN_H
#define SCREEN_H

#include "LayoutTree.h"
#include "Rendering\RenderWindow.h"

namespace ViWm
{
    /**
     * Represent a display.
     * Now dual or more screen are usual for programmers,
     * we must handle it nicely.
     */
    struct Screen 
    {
        Screen( Renderer::RenderWindow &associated
              , int x, int y, int width, int height );

        ~Screen();

        // This accessor are a bit ugly, but I don't
        // want some change screen size by mistake.
        int getX()      { return size.x; }
        int getY()      { return size.y; }
        int getWidth()  { return size.width; }
        int getHeight() { return size.height; }

        void   replace();

        LayoutTree::SplitSide   initialSplit;
        LayoutTree              *layoutRoot;

    private:
        //void    operator =( const Screen& );

        Rect                            size;
        Renderer::RenderWindow          *fullScreenWin;
    };

    typedef std::vector< Screen >   DesktopLayout;
}

#endif /* SCREEN_H */
