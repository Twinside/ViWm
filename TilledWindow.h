#ifndef TILLEDWINDOW_H
#define TILLEDWINDOW_H

#ifdef WIN32
#   include <Windows.h>
#endif
#include <string>
#include "LayoutTree.h"

namespace ViWm
{
    class TilledWindow
    {
    public:
        TilledWindow(LayoutTree::WindowKey realWindow);
        ~TilledWindow();

        void SetTransparency(int alpha);
        void SetBorders(bool presentOrNot);
        void GiveFocus();
        void SetSize( int x, int y, int width, int height );

        void GetClassName( std::wstring &name );

        bool IsTileable();
        static bool IsHandleTileable( LayoutTree::WindowKey hwnd );

        unsigned short  tags;

        bool    operator == ( LayoutTree::WindowKey otherWindow ) const
            { return hwnd == otherWindow; }

        bool    operator != ( LayoutTree::WindowKey otherWindow ) const
            { return hwnd != otherWindow; }

        /**
         * If the window has been destroyed,
         * ensure no operations will be done
         * one the window.
         */
        void    Discard() { hwnd = NULL; }
        LayoutTree::WindowKey getWinowKey() const
            { return hwnd; }

        struct Finder {
            LayoutTree::WindowKey  toFind;
            Finder( LayoutTree::WindowKey ntoFind )
                : toFind( ntoFind ) {}

            inline bool   operator() ( const TilledWindow* window )
                { return *window == toFind; }
        };

    private:
        enum Constants
        {
            Opaque = 255
        };

        LayoutTree::WindowKey    hwnd;
        int     applyedAlpha;
        bool    hasBorder;
        RECT    initialSize;

        static TCHAR TempClassName[MAX_PATH];
    };
}

#endif /* TILLEDWINDOW_H */
