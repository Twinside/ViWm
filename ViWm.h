#ifndef VIWM_H
#define VIWM_H

#include <vector>
#include <memory>
#include "TilledWindow.h"
#include "Screen.h"
#include "State.h"

class Action;
class Layouter;

typedef int VirtualKey;
typedef std::pair<VirtualKey, Action*> HotKey;
typedef std::vector<HotKey> HotKeyCollection;

class ViWm
{
public:
    ViWm( HINSTANCE hInstance
        , const HotKeyCollection  &originalConfiguration
        );
    ~ViWm();

    void    Init();

    void    AddNode( HWND hwnd );
    void    ArrangeWindows();
    void    FocusCurrent();


    LRESULT HandleShellHook( HWND hwnd, UINT msg
                           , WPARAM wParam, LPARAM lParam );
    void    HandleHotKey( WPARAM wParam );

private:
    // DO NOT WANT!!
    void    operator =( const ViWm& );
    ViWm( const ViWm & );

    int modkeys;

    void    UnregisterHotKeys( HWND hwnd );
    void    RegisterHotKeys( HWND hwnd );

    static BOOL CALLBACK
             monitorEnumerator( HMONITOR hMonitor
                              , HDC      hdcMonitor
                              , LPRECT   intersectionRect
                              , LPARAM   userData );

    void    createGlobalListener( HINSTANCE hInstance );

    typedef std::tr1::shared_ptr<Layouter>
            LayoutPtr;

    HotKeyCollection        hotkeysDefinition;
    std::vector<LayoutPtr>  layouter;

    
    HINSTANCE               hInstance;
    DesktopLayout           currentLayout;
    WindowMakerState        currentState;
    HWND                    globalHotkeyListener;
    UINT                    shellhookid;	/* Window Message id */

    char ignoreClasses[MAX_IGNORE][128];		/* Don't include these classes in tiling */
    char ignoreClassesBorders[MAX_IGNORE][128]; 	/* Don't remove borders from these classes */
};

#endif /* VIWM_H */
