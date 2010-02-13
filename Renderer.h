#ifndef RENDERER_H
#define RENDERER_H

#include <d2d1.h>

namespace ViWm
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        typedef ID2D1SolidColorBrush* Brush;

        Brush   CreateBrush( float r, float g, float b, float a );
        void    DeleteBrush( Brush b );

        void    begin();
        void    drawRect( Brush color, int x, int y, int width, int height );
        void    end();

        
    private:
        HWND m_hwnd;
        ID2D1Factory* m_pDirect2dFactory;
        ID2D1HwndRenderTarget* m_pRenderTarget;
    };
}

#endif /* RENDERER_H */
