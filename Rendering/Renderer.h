#ifndef RENDERER_H
#define RENDERER_H

#include <d2d1.h>

struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;
namespace ViWm {
namespace Renderer
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        typedef ID2D1SolidColorBrush* Brush;

        Brush   CreateBrush( float r, float g, float b, float a );
        void    DeleteBrush( Brush b );

        RenderWindow
        void    begin();
        void    drawRect( Brush color, int x, int y, int width, int height );
        void    end();

        
    private:
        ID2D1Factory            *m_pDirect2dFactory;
        IWICImagingFactory      *factory;
    };
}}

#endif /* RENDERER_H */
