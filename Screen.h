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
        /** 
         * Represent a physical screen.
         * The coordinate (x,y) can be negative as we need
         * to handle the multi view scenario.
         * @param associated Render window used to display splits.
         * @param x horizontal position of upper left corner
         * @param y vertical position of upper left corner
         * @param width in pixels
         * @param height in pixels
         */
        Screen( Renderer::RenderWindow &associated
              , int x, int y, int width, int height );

        ~Screen();

        // This accessor are a bit ugly, but I don't
        // want some change screen size by mistake.
        int getX() const { return size.x; }
        int getY() const { return size.y; }
        int getWidth() const { return size.width; }
        int getHeight() const { return size.height; }

        bool   isInScreenBound( int x, int y ) const;
        LayoutTree::SplitCoord    FindPointedSplit( int x, int y );

        void   replace( bool transparentVoid );

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
