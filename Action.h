#ifndef ACTION_H
#define ACTION_H

#include "ViWm.h"

namespace ViWm
{
    typedef int StringId;
    enum ActionConstants
    {
        NoStringId = -1
    };

    /**
     * Class associated with a real action.
     */
    class Action
    {
    public:
        Action( StringId displayName
              , StringId description )
              : displayId( displayName ), descrId( description )
        {}

        enum ReturnInfo
        {
            NeedRelayout,
            Nothing
        };

        StringId    getDisplayName() const { return displayId; }
        StringId    getDescription() const { return descrId; }

        virtual ReturnInfo operator() ( DesktopLayout       &l
                                      , WindowMakerState    &state
                                      ) = 0;
    private:
        StringId    displayId;
        StringId    descrId;
    };
}

#endif /* ACTION_H */
