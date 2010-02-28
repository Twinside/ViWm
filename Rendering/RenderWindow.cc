#include <assert.h>
#include "RenderWindow.h"

namespace ViWm {
namespace Renderer
{
    RenderWindow::RenderWindow( HWND window, int x, int y, int width, int height )
        : m_info( x, y, width, height )
        , m_window( window )
    {
        transparentColor = CreateBrush(0,128,0,128);
    }
    
    RenderWindow::~RenderWindow()
    {
        DeleteBrush( transparentColor );
    }

    void RenderWindow::drawRect( Brush color, int x, int y, int width, int height )
    {
        RECT    myRect = { x, y, width, height };
        int ret = FillRect( memDC, &myRect, color );

        assert( ret != 0 );
    }

    void RenderWindow::begin()
    {
        int err;
        int width = m_info.GetWidth();
        int height = m_info.GetHeight();
        RECT fullRect = { 0, 0, width, height };

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
        //*/

        //bitmap = CreateCompatibleBitmap( memDC, width, height );
        bitmap = CreateDIBSection( memDC, &bitmapInfo, DIB_RGB_COLORS, &voidBits,  NULL, 0 );
        err = GetLastError();
        if (bitmap == NULL) throw;

        oldBitmap = (HBITMAP)SelectObject( memDC, bitmap );
        err = GetLastError();

        assert( oldBitmap != 0 );
        int ret = FillRect( memDC, &fullRect, transparentColor );
        err = GetLastError();
        assert( ret != 0 );
    }

    void RenderWindow::end()
    {
        m_info.Update( m_window, memDC );
        ReleaseDC( NULL, screenDC );
        if ( bitmap )
        {
            SelectObject( memDC, oldBitmap );
            DeleteObject( bitmap );
        }
        DeleteDC( memDC );
    }

    void RenderWindow::DeleteBrush( Brush b )
        { DeleteObject( b ); }

    RenderWindow::Brush RenderWindow::CreateBrush( int r, int g, int b, int a )
    {
        COLORREF    intColor = RGB(r, g, b) | a << 24;
        HBRUSH created = CreateSolidBrush( intColor );
        assert( created != NULL );
        return created;
    }
}}
