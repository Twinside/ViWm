#include "RenderWindow.h"
#include "RenderTargetDC.h"

namespace ViWm {
namespace Renderer
{

    RenderWindow::RenderWindow( HWND window
                              , int width, int height
                              , Renderer::RenderTarget &target )
        : m_info( width, height )
        , m_window( window )
        , m_pRenderTarget( &target )
        , m_interopRenderTarget( &target )
    {}
    
    RenderWindow::~RenderWindow()
    {
    }

    void RenderWindow::drawRect( Brush color, int x, int y, int width, int height )
    {
        D2D1_RECT_F rectangle2 = D2D1::RectF( float(x)
            , float(y)
            , float(x + width)
            , float(x + height) );

        m_pRenderTarget->FillRectangle( rectangle2, color );
    }

    void RenderWindow::begin()
    {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform( D2D1::Matrix3x2F::Identity() );
    }

    void RenderWindow::end()
    {
        m_info.Update( m_window, m_interopRenderTarget );
        m_pRenderTarget->EndDraw();
    }

    void RenderWindow::DeleteBrush( Brush b )
        { if ( b ) b->Release(); }

    RenderWindow::Brush RenderWindow::CreateBrush( float r, float g, float b, float a )
    {
        if ( !m_pRenderTarget ) throw;

        Brush   retBrush;
        m_pRenderTarget->CreateSolidColorBrush( D2D1::ColorF(r,g,b,a)
                                              , &retBrush);

        return retBrush;
    }
}}
