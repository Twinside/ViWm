#ifndef __LAYEREDWINDOWINFO_H__
#define __LAYEREDWINDOWINFO_H__

#ifdef WIN32
#   include <windows.h>
#endif

namespace ViWm {
namespace Renderer
{
    class LayeredWindowInfo
    {
    public:
        LayeredWindowInfo( int x, int y, UINT width, UINT height);
        void Update( LayoutTree::WindowKey window, HDC source);  
        UINT GetWidth() const { return m_size.cx; }
        UINT GetHeight() const { return m_size.cy; }

        int GetX() const { return m_windowPosition.x; }
        int GetY() const { return m_windowPosition.y; }

    private:
        void    operator= (const LayeredWindowInfo&);

        POINT                   m_sourcePosition;
        POINT                   m_windowPosition;
        SIZE                    m_size;
        BLENDFUNCTION           m_blend;
        UPDATELAYEREDWINDOWINFO m_info;
    };
}}

#endif /* __LAYEREDWINDOWINFO_H__ */

