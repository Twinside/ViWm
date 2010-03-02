#ifndef TILLEDWINDOW_H
#define TILLEDWINDOW_H

#include <Windows.h>
#include <string>

namespace ViWm
{
    class TilledWindow
    {
    public:
        TilledWindow(HWND   realWindow);
        ~TilledWindow();

        void SetTransparency(int alpha);
        void SetBorders(bool presentOrNot);
        void GiveFocus();
        void SetSize( int x, int y, int width, int height );

        void GetClassName( std::string &name );

        bool IsTileable();
        static bool IsHandleTileable( HWND hwnd );

        unsigned short  tags;

        bool    operator == ( HWND otherWindow ) const
            { return hwnd == otherWindow; }

        bool    operator != ( HWND otherWindow ) const
            { return hwnd != otherWindow; }

        /**
         * If the window has been destroyed,
         * ensure no operations will be done
         * one the window.
         */
        void    Discard() { hwnd = NULL; }
        HWND    getWinowKey() { return hwnd; }

        struct Finder {
            HWND  toFind;
            Finder( HWND ntoFind ) : toFind( ntoFind ) {}

            inline bool   operator() ( const TilledWindow* window )
                { return *window == toFind; }
        };

    private:
        enum Constants
        {
            Opaque = 255
        };

        HWND    hwnd;
        int     applyedAlpha;
        bool    hasBorder;

        static char TempClassName[MAX_PATH];
    };
}

#endif /* TILLEDWINDOW_H */
