#ifndef ACTION_H
#define ACTION_H

#include "ViWm.h"

/**
 * Class associated with a real action.
 */
class Action
{
public:
    enum ReturnInfo
    {
        NeedRelayout,
        Nothing
    };

    virtual ReturnInfo operator() ( DesktopLayout       &l
                                  , WindowMakerState    &state
                                  ) = 0;
};

#endif /* ACTION_H */
