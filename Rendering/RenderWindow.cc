#include <wincodec.h>
#include <D2D1.h>
#include "RenderWindow.h"

namespace ViWm {
namespace Renderer
{
    const D2D1_PIXEL_FORMAT format = D2D1::PixelFormat(
                          DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED);

    const D2D1_RENDER_TARGET_PROPERTIES properties = 
        D2D1::RenderTargetProperties( D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                     format,
                                     0.0f, // default dpi
                                     0.0f, // default dpi
                                     D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

    RenderWindow::RenderWindow( HWND window )
    {
        IWICImagingFactory *factory;
        HRESULT rez;

        rez = CoCreateInstance( CLSID_WICImagingFactory, NULL
                              , CLSCTX_FROM_DEFAULT_CONTEXT
                              , IID_IWICImagingFactory
                              , (void**)&factory );
        if (FAILED(rez)) throw;

        IWICBitmap *bitmap;

        rez = factory->CreateBitmap( m_info.GetWidth(),
                                     m_info.GetHeight(),
                                     GUID_WICPixelFormat32bppPBGRA,
                                     WICBitmapCacheOnLoad,
                                     &bitmap);

        if ( FAILED(rez) )
        {
            factory->Release();
            throw;
        }

        ID2D1RenderTarget *target;

        rez = factory->CreateWicBitmapRenderTarget( bitmap
                                                  , properties
                                                  , &target);

        if (FAILED(rez))
        {
            factory->Release();
            bitmap->Release();
        }
    }
    
    RenderWindow::~RenderWindow()
    {
    }
}}

