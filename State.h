#ifndef STATE_H
#define STATE_H

#include <list>
#include "Constants.h"

class WindowMakerState
{
public:
    bool    disableNext;
    bool    lockMouse;

    uint16 experimental_mouse;
    uint16 mouse_pos_out;
    int margin;

    TillingMode tilingMode;

    uint16 alpha;
    uint16 borders;
    uint16 ignoreCount;
    uint16 ignoreCountBorders;

    /* Xmonad style Master area count */
    uint16 masterarea_count;

    /* Should always point to current node */
    TilledWindow *current; 	

    std::list< TilledWindow* >  windowList;
};

#endif /* STATE_H */