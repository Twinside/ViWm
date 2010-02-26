#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#include <D2D1.h>
#include "LayeredWindowInfo.h"
#include "Renderer.h"

namespace ViWm {
namespace Renderer
{
    class RenderWindow
    {
    public:
        RenderWindow( HWND window, int width, int height
                    , Renderer::RenderTarget &target
                    );

        ~RenderWindow();

        typedef ID2D1SolidColorBrush* Brush;

        Brush   CreateBrush( float r, float g, float b, float a );
        void    DeleteBrush( Brush b );


        void    begin();
        void    drawRect( Brush color, int x, int y, int width, int height );
        void    end();
        
    private:
        RenderWindow( const RenderWindow &cpy );
        HWND                    m_window;
        LayeredWindowInfo       m_info;
        Renderer::RenderTarget  *m_pRenderTarget;
        RenderTargetDC          m_interopRenderTarget;
    };
}}

#endif /* __RENDERWINDOW_H__ */

