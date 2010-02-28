#include "LayeredWindowInfo.h"

namespace ViWm {
namespace Renderer
{
    LayeredWindowInfo::LayeredWindowInfo( int x, int y, UINT width, UINT height)
        : m_sourcePosition()
        , m_windowPosition()
        , m_blend()
        , m_info()
    {
        m_size.cx = width;
        m_size.cy = height;

        m_sourcePosition.x = 0;
        m_sourcePosition.y = 0;

        m_windowPosition.x = x;
        m_windowPosition.y = y;

        m_blend.BlendOp = AC_SRC_OVER;
        m_blend.BlendFlags = 0;
        m_blend.SourceConstantAlpha = 255;
        m_blend.AlphaFormat = AC_SRC_ALPHA;

        m_info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
        m_info.pptSrc = &m_sourcePosition;
        m_info.pptDst = &m_windowPosition;
        m_info.psize = &m_size;
        m_info.pblend = &m_blend;
        m_info.dwFlags = ULW_ALPHA;
    }

    void LayeredWindowInfo::Update( HWND window, HDC source)
    {
        m_info.hdcSrc = source;
        BOOL rez = UpdateLayeredWindowIndirect(window, &m_info);

        if (rez == 0)
        {
            DWORD errorCode = GetLastError();
            throw;
        }
    }
}}
