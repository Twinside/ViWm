#ifndef CONSTANTS_H
#define CONSTANTS_H

#if (defined WIN32) && !(defined TEST)
#   define REALCODE
#endif

#ifdef REALCODE
#   include <windows.h>
#else
#   include "WinTypeTestSet.h"
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
