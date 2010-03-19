#ifndef CONSTANTS_H
#define CONSTANTS_H

#if (defined WIN32) && !(defined TEST)
#   define REALCODE
#endif

#ifdef REALCODE
#   include <windows.h>
#else

typedef char* LPTSTR;
typedef int   HWND;
typedef int   COLORREF;
typedef char  TCHAR;
typedef int   HINSTANCE;
typedef int   LRESULT;
typedef unsigned int UINT;
typedef int   WPARAM;
typedef int   LPARAM;
typedef int   BOOL;

struct RECT
{
    int left;
    int right;
    int top;
    int bottom;
};

typedef RECT*   LPRECT;
typedef int     HDC;
typedef int     HMONITOR;

#define CALLBACK
#define MAX_PATH    512
#define MOD_CONTROL 0
#define MOD_ALT     1
#endif

#ifdef MSVC
#   pragma warning( disable : 4100 )
#endif

enum    Tags
{
    TAG_1 = 1,
    TAG_2 = 2,
    TAG_3 = 4,
    TAG_4 = 8,
    TAG_5 = 16,
    LastTag = 5
};

enum    ConfigurationConstants
{
    DEFAULT_MODKEY = MOD_CONTROL | MOD_ALT,
};

extern const LPTSTR keyListenerClassName;
extern const LPTSTR fullScreenWindowClassName;

namespace ViWm
{
    namespace Renderer { typedef COLORREF    Brush; }
}

#endif /* CONSTANTS_H */
