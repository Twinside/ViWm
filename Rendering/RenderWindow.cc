#include <assert.h>
#include "RenderWindow.h"

namespace ViWm {
namespace Renderer
{
    RenderWindow::RenderWindow( HWND window, int x, int y, int width, int height )
        : m_info( x, y, width, height )
        , m_window( window )
    {
        transparentColor = CreateBrush(0,0,0, 0);
        nearlyTransparent = CreateBrush( 0, 0, 255, 64);
    }
    
    RenderWindow::~RenderWindow()
    {
        DeleteBrush( transparentColor );
    }

    void RenderWindow::drawRect( Brush color, int x, int y, int width, int height )
    {
        UINT     winHeight = m_info.GetHeight();
        UINT     winWidth = m_info.GetWidth();

        assert( x >= 0 );
        assert( UINT(x) < winWidth );
        assert( y >= 0 );
        assert( UINT(y) < winHeight );

        assert( UINT(x + width) <= winWidth );
        assert( UINT(y + height) <= winHeight );

        // because in the bitmap we have the head
        // flipped compared to the up of the screen
        // being 0.
        y = winHeight - (y + height);

        for (int yi = 0; yi < height; yi++)
        {
            for (int xi = 0; xi < width; xi++)
            {
                voidBits[x + xi + (y + yi) * winWidth] = color;
            }
        }
    }

    void RenderWindow::begin( bool fullyTransparent )
    {
        int err;
        int width = m_info.GetWidth();
        int height = m_info.GetHeight();

        screenDC = GetDC( NULL );
        err = GetLastError();
        memDC = CreateCompatibleDC( screenDC );
        err = GetLastError();

        if (memDC == NULL) throw;

        BITMAPINFO bitmapInfo = { };
        bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
        bitmapInfo.bmiHeader.biWidth = width;
        bitmapInfo.bmiHeader.biHeight = height;
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;

        bitmap = CreateDIBSection( memDC, &bitmapInfo, DIB_RGB_COLORS, (void**)&voidBits,  NULL, 0 );
        err = GetLastError();
        if (bitmap == NULL) throw;

        oldBitmap = (HBITMAP)SelectObject( memDC, bitmap );
        err = GetLastError();

        assert( oldBitmap != 0 );

        drawRect( nearlyTransparent, 0, 0, width, 1 );

        if ( fullyTransparent )
            drawRect( transparentColor, 0, 0, width, height );
        else
            drawRect( nearlyTransparent, 0, 0, width, height );
    }

    void RenderWindow::end()
    {
        GdiFlush();
        m_info.Update( m_window, memDC );
        ReleaseDC( NULL, screenDC );
        if ( bitmap )
        {
            SelectObject( memDC, oldBitmap );
            DeleteObject( bitmap );
        }
        DeleteDC( memDC );
    }

    RenderWindow::Brush RenderWindow::CreateBrush( int r, int g, int b, int a )
    {
        assert( a >= 0 && a <= 255 );
        assert( r >= 0 && r <= 255 );
        assert( g >= 0 && g <= 255 );
        assert( b >= 0 && b <= 255 );

        int         preR = r * a / 255;
        int         preG = g * a / 255;
        int         preB = b * a / 255;

        COLORREF    intColor = preB
                             | preG << 8
                             | preR << 16
                             | a << 24;

        return intColor;
    }

    void RenderWindow::putAtTop()
    {
        SetWindowPos( m_window, HWND_TOP
                    , m_info.GetX()
                    , m_info.GetY()
                    , m_info.GetWidth(), m_info.GetHeight()
                    , SWP_NOMOVE | SWP_NOSIZE
                    );
    }

    void RenderWindow::putAtBottom()
    {
        SetWindowPos( m_window, HWND_BOTTOM
                    , m_info.GetX()
                    , m_info.GetY()
                    , m_info.GetWidth(), m_info.GetHeight()
                    , SWP_NOMOVE | SWP_NOSIZE
                    );
    }
}}
