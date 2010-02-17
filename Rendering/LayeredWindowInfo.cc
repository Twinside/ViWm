#include "LayeredWindowInfo.h"

namespace ViWm {
namespace Renderer
{
    LayeredWindowInfo::LayeredWindowInfo( UINT width, UINT height)
        : m_sourcePosition()
        , m_windowPosition()
        , m_blend()
        , m_info()
    {
        m_size.cx = width;
        m_size.cy = height;

        m_info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
        m_info.pptSrc = &m_sourcePosition;
        m_info.pptDst = &m_windowPosition;
        m_info.psize = &m_size;
        m_info.pblend = &m_blend;
        m_info.dwFlags = ULW_ALPHA;

        m_blend.SourceConstantAlpha = 255;
        m_blend.AlphaFormat = AC_SRC_ALPHA;
    }

    void LayeredWindowInfo::Update( HWND window, HDC source)
    {
        m_info.hdcSrc = source;
        UpdateLayeredWindowIndirect(window, &m_info);
    }
}}
