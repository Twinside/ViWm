#include "Renderer.h"

namespace ViWm {
namespace Renderer
{
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
        : m_hwnd(NULL)
        , m_pDirect2dFactory(NULL)
        , m_pRenderTarget(NULL)

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

        m_hwnd = GetDesktopWindow();

        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        hr = m_pDirect2dFactory->CreateHwndRenderTarget
                ( D2D1::RenderTargetProperties()
                , D2D1::HwndRenderTargetProperties(m_hwnd, size)
                , &m_pRenderTarget );
    }

    Renderer::~Renderer()
    {
        SafeRelease(&m_pDirect2dFactory);
        SafeRelease(&factory);
    }

    void Renderer::drawRect( Brush color, int x, int y, int width, int height )
    {
        D2D1_RECT_F rectangle2 = D2D1::RectF( float(x)
                                            , float(y)
                                            , float(x + width)
                                            , float(x + height) );

        m_pRenderTarget->FillRectangle( rectangle2, color );
    }

    void Renderer::begin()
    {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform( D2D1::Matrix3x2F::Identity() );
    }

    void Renderer::end()
    {
        m_pRenderTarget->EndDraw();
    }

    void Renderer::DeleteBrush( Brush b )
        { SafeRelease( &b ); }

    Renderer::Brush Renderer::CreateBrush( float r, float g, float b, float a )
    {
        if ( !m_pRenderTarget ) throw;

        Brush   retBrush;
        m_pRenderTarget->CreateSolidColorBrush( D2D1::ColorF(r,g,b,a)
                                              , &retBrush);

        return retBrush;
    }
}}
