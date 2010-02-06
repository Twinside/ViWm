#ifndef VIWM_H
#define VIWM_H

#include <vector>
#include "TilledWindow.h"
#include "Screen.h"
#include "State.h"

class DesktopLayout
{
public:
    std::vector< Screen >   displays;
};

class ViWm
{
public:
    ViWm(void);
    ~ViWm(void);

    void    ArrangeWindows();
    void    RegisterHotKeys( HWND hwnd );

    void    AddNode( HWND hwnd );

private:
    int modkeys;

    static BOOL monitorEnumerator( HMONITOR hMonitor
                                 , HDC      hdcMonitor
                                 , LPRECT   intersectionRect
                                 , LPARAM   userData );

    DesktopLayout           currentLayout;
    WindowMakerState        currentState;

    char ignoreClasses[MAX_IGNORE][128];		/* Don't include these classes in tiling */
    char ignoreClassesBorders[MAX_IGNORE][128]; 	/* Don't remove borders from these classes */
};


#endif /* VIWM_H */
