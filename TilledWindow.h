#ifndef TILLEDWINDOW_H
#define TILLEDWINDOW_H

#include <Windows.h>

class TilledWindow
{
public:
    TilledWindow(HWND   realWindow);
    ~TilledWindow();

    void SetTransparency(int alpha);
    void SetBorders(bool presentOrNot);

    bool IsTileable();
    static bool IsHandleTileable();

    unsigned short  tags;

private:
    enum Constants
    {
        Opaque = 255
    };

    HWND    hwnd;
    int     applyedAlpha;
    bool    hasBorder;
};

#endif /* TILLEDWINDOW_H */
