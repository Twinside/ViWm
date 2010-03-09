#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <windows.h>

#pragma warning( disable : 4100 )

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

#endif /* CONSTANTS_H */
