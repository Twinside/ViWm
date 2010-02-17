#ifndef __LAYEREDWINDOWINFO_H__
#define __LAYEREDWINDOWINFO_H__

#include <windows.h>

namespace ViWm {
namespace Renderer
{
    class LayeredWindowInfo
    {
    public:
        LayeredWindowInfo( UINT width, UINT height);
        void Update( HWND window, HDC source);  
        UINT GetWidth() const { return m_size.cx; }
        UINT GetHeight() const { return m_size.cy; }

    private:
        void    operator= (const LayeredWindowInfo&);

        const POINT             m_sourcePosition;
        POINT                   m_windowPosition;
        SIZE                    m_size;
        BLENDFUNCTION           m_blend;
        UPDATELAYEREDWINDOWINFO m_info;
    };
}}

#endif /* __LAYEREDWINDOWINFO_H__ */

