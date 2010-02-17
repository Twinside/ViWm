#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#include <wincodec.h>
#include <D2D1.h>
#include "LayeredWindowInfo.h"

namespace ViWm {
namespace Renderer
{
    class RenderWindow
    {
    public:
        RenderWindow( HWND window );
        ~RenderWindow();
        
    private:
        RenderWindow( const RenderWindow &cpy );
        LayeredWindowInfo   m_info;
        ID2D1RenderTarget   *renderTarget;
        IWICImagingFactory  *factory;
    };
}}

#endif /* __RENDERWINDOW_H__ */

