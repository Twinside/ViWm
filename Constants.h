#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <windows.h>

enum    Tags
{
    TAG_1 = 1,
    TAG_2 = 2,
    TAG_3 = 4,
    TAG_4 = 8,
    TAG_5 = 16
};

enum TillingMode
{
    VerticalTilling,
    HorizontalTilling,
    GridTilling,
    FullScreenTilling,
    LastTillingMode
};

enum    ConfigurationConstants
{
    DEFAULT_MODKEY = MOD_CONTROL | MOD_ALT,
    MAX_IGNORE = 16,
    MemoryTrimmDelay = 60000,
    MousePollDelay = 500
};

const char* NAME;
const char* VERSION;

typedef unsigned short uint16;

#endif /* CONSTANTS_H */
