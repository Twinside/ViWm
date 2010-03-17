#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#ifdef WIN32
#   include "LayeredWindowInfo.h"
#endif
#include "../LayoutTree.h"
#include "../Constants.h"

namespace ViWm {
namespace Renderer
{
#if (defined WIN32) && !(defined TEST)
    class RenderWindow
    {
    public:
        RenderWindow( LayoutTree::WindowKey
                    , window, int x, int y, int width, int height );

        ~RenderWindow();

        Brush   CreateBrush( int r, int g, int b, int a );
        void    DeleteBrush( Brush /*b*/ ) const {}


        void    putAtTop();
        void    putAtBottom();

        void    begin( bool fullyTransparent );
        void    drawRect( Brush color, int x, int y, int width, int height );
        void    end();
        
    private:
        RenderWindow( const RenderWindow &cpy );

        HDC                     screenDC;
        HDC                     memDC;
        HBITMAP                 bitmap, oldBitmap;

        HWND                    m_window;
        LayeredWindowInfo       m_info;
        Brush                   transparentColor;
        Brush                   nearlyTransparent;
        COLORREF                *voidBits;
    };
#else
    class RenderWindow
    {
    public:
        RenderWindow( LayoutTree::WindowKey window
                    , int x, int y, int width, int height );

        ~RenderWindow();

        Brush   CreateBrush( int /*r*/, int /*g*/
                           , int /*b*/, int /*a*/ )
            { return 0; }
        void    DeleteBrush( Brush /*b*/ ) const {}


        void    putAtTop() {}
        void    putAtBottom() {}

        void    begin( bool /*fullyTransparent*/ ) {}
        void    drawRect( Brush /*color*/
                        , int /*x*/, int /*y*/
                        , int /*width*/, int /*height*/ )
            {}
        void    end() {}
    };
#endif
}}

#endif /* __RENDERWINDOW_H__ */

