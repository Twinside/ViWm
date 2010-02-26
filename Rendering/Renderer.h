#ifndef RENDERER_H
#define RENDERER_H

#include <wincodec.h>
#include <d2d1.h>

struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;

namespace ViWm {
namespace Renderer
{
    class RenderWindow;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        typedef ID2D1RenderTarget RenderTarget;

        RenderWindow&   CreateRenderWindow( HWND window );
        
    private:
        ID2D1Factory            *m_pDirect2dFactory;
        IWICImagingFactory      *factory;
    };
}}

#endif /* RENDERER_H */
