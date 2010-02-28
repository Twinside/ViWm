#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#include "LayeredWindowInfo.h"

namespace ViWm {
namespace Renderer
{
    class RenderWindow
    {
    public:
        RenderWindow( HWND window, int x, int y, int width, int height );

        ~RenderWindow();

        typedef HBRUSH Brush;

        Brush   CreateBrush( int r, int g, int b, int a );
        void    DeleteBrush( Brush b );


        void    begin();
        void    drawRect( Brush color, int x, int y, int width, int height );
        void    end();
        
    private:
        RenderWindow( const RenderWindow &cpy );

        HDC                     screenDC;
        HDC                     memDC;
        HBITMAP                 bitmap, oldBitmap;

        HWND                    m_window;
        LayeredWindowInfo       m_info;
        HBRUSH                  transparentColor;
        void                    *voidBits;
    };
}}

#endif /* __RENDERWINDOW_H__ */

