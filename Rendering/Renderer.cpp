#include "Renderer.h"
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
    template<class Interface>
    inline void SafeRelease( Interface **ppInterfaceToRelease )
    {
        if (*ppInterfaceToRelease != NULL)
        {
            (*ppInterfaceToRelease)->Release();
            (*ppInterfaceToRelease) = NULL;
        }
    }

    Renderer::Renderer()
        : m_pDirect2dFactory(NULL)
        , factory( NULL )
    {
        HRESULT hr;
        hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED
                              , &m_pDirect2dFactory);

        if (! SUCCEEDED(hr)) throw;

        hr = CoCreateInstance( CLSID_WICImagingFactory, NULL
                             , CLSCTX_INPROC_SERVER
                             , IID_IWICImagingFactory
                             , (void**)&factory );

        if (! SUCCEEDED(hr)) throw;
    }

    Renderer::~Renderer()
    {
        SafeRelease(&m_pDirect2dFactory);
        SafeRelease(&factory);
    }

    RenderWindow& Renderer::CreateRenderWindow( HWND window )
    {
        RenderTarget    *target;
        IWICBitmap      *bitmap;
        //ID2D1GdiInteropRenderTarget    *interopTarget;
        WINDOWINFO      winInfo;

        winInfo.cbSize = sizeof( WINDOWINFO );
        GetWindowInfo( window, &winInfo );

        int width = winInfo.rcWindow.right - winInfo.rcWindow.left;
        int height = winInfo.rcWindow.bottom - winInfo.rcWindow.top;

        factory->CreateBitmap
            ( width
            , height
            , GUID_WICPixelFormat32bppPBGRA
            , WICBitmapCacheOnLoad
            , &bitmap);

        m_pDirect2dFactory->CreateWicBitmapRenderTarget(
            bitmap,
            properties,
            &target);

        return *new RenderWindow( window, width, height, *target );
    }
}}
