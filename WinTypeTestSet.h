#ifndef __WINTYPETEST_H__
#    define __WINTYPETEST_H__

#ifndef REALCODE
// All this define is used to bypass windows type
// and "mock" the system with light types
#    define LPTSTR char*
#    define HWND int
#    define COLORREF int
#    define TCHAR char
#    define HINSTANCE int
#    define LRESULT int
#    define UINT unsigned int
#    define WPARAM int
#    define LPARAM int
#    define BOOL int

struct myTestVerySpecialRECT
{
    int left;
    int right;
    int top;
    int bottom;
};

#    define RECT myTestVerySpecialRECT

#    define LPRECT RECT*
#    define HDC int
#    define HMONITOR int

#    define CALLBACK
#    define MAX_PATH    512
#    define MOD_CONTROL 0
#    define MOD_ALT     1

#endif /* REALCODE */

#endif /* __WINTYPETEST_H__ */
