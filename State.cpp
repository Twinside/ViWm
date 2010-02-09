#include <algorithm>
#include "TilledWindow.h"
#include "State.h"

WindowMakerState::WindowMakerState()
    : tilingMode( ManualVimTilling )
    , alpha( 255 )
    , currentScreen( 0 )
    //, borders( 1 )
    //, ignoreCount( 0 )
    //, ignoreCountBorders( 0 )
    //, experimental_mouse( 0 )
    //, mouse_pos_out( 0 )
    //, margin( 120 ), masterarea_count( 1 )
{}

TilledWindow* WindowMakerState::FindNode( HWND hwnd )
{
    std::vector<Bucket>::iterator it;
    std::list<TilledWindow*>::iterator     found;
    TilledWindow::Finder                   comparer( hwnd );

    // foreach it in windowList
    for ( it = windowList.begin()
        ; it != windowList.end()
        ; ++it )
    {
        found = std::find_if( it->windowList.begin()
                            , it->windowList.end()
                            , comparer );

        // we have found something
        if ( found != it->windowList.end() )
            return *found;
    }

    return 0;
}

void WindowMakerState::RemoveNode( HWND hwnd )
{
    std::vector<Bucket>::iterator it;
    std::list<TilledWindow*>::iterator     firstFound;
    std::list<TilledWindow*>::iterator     found;
    bool                                   hasFound = false;
    TilledWindow::Finder                   comparer( hwnd );

    // foreach it in windowList
    for ( it = windowList.begin()
        ; it != windowList.end()
        ; ++it )
    {
        // *sigh* I need lambdas.
        found = std::find_if( it->windowList.begin()
                            , it->windowList.end()
                            , comparer );

        // we have found something
        if ( found != it->windowList.end() )
        {
            if ( !hasFound )
            {
                firstFound = found;
                hasFound = true;
            }

            it->windowList.erase( found );
        }
    }

    if ( hasFound )
        delete *firstFound;
}
