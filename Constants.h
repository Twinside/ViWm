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
    MAX_IGNORE = 16,
    MemoryTrimmDelay = 60000,
    MousePollDelay = 500
};

typedef unsigned short uint16;

#endif /* CONSTANTS_H */
