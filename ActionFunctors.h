#ifndef ACTIONFUNCTORS_H
#define ACTIONFUNCTORS_H

#include "Action.h"
#include "Constants.h"

namespace ViWm {
namespace Actions
{
    struct VoidAction : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct ReturnInfoAction : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct SelectUp : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct SelectDown : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MoveToMainArea : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MarginMove : public Action
    {
    public:
        MarginMove( int namount );
        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        int amount;
    };

    struct MarginLeft : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MarginRight : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct IgnoreMode : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MouseLock : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct SwitchTilling : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MoveWindowUp : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct MoveWindowDown : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct ShowWindowClass : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct TileWindow : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    struct UntileWindow : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };

    class SwitchTag : public Action
    {
    public:
        SwitchTag( Tags ntoTag );

        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        Tags toTag;
    };

    struct MoveTag : public Action
    {
    public:
        MoveTag( Tags ntoTag );
        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        Tags toTag;
    };

    class MasterAreaChange : public Action
    {
    public:
        MasterAreaChange( int namount );
        ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s);

    private:
        int amount;
    };

    struct CloseForeground : public Action
        { ReturnInfo operator() ( DesktopLayout &l, WindowMakerState &s); };
}}

#endif /* ACTIONFUNCTORS_H */
