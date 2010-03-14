#include <algorithm>
#include "TilledWindow.h"
#include "State.h"
#include "Layouter/Layouts.h"
#include "Configuration.h"

namespace ViWm
{
    WindowMakerState::WindowMakerState( const Configuration &conf )
        : tilingMode( ManualVimTilling )
        , currentScreen( 0 )
        , currentTag( 0 )
        , current( 0 )
    {
        windowList.resize( LastTag );

        layouter.reserve( LastTillingMode );
        layouter.push_back( LayoutPtr( new Layout::ManualVimLayout( conf ) ) );
    }

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
        std::list<TilledWindow*>::iterator     found;
        TilledWindow*                          firstFound = 0;
        bool                                   hasFound = false;
        TilledWindow::Finder                   comparer( hwnd );

        // if the current selected window was selected,
        // and is destroyed, don't forget to remove it's
        // reference. (5h bug fix)
        if ( current && (*current) == hwnd )
            current = 0;

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
                    firstFound = *found;
                    hasFound = true;
                }

                it->windowList.erase( found );
            }
        }

        if ( hasFound )
        {
            firstFound->Discard();
            delete firstFound;
        }
    }

    WindowMakerState::~WindowMakerState()
    {
        std::vector<Bucket>::iterator it;
        std::list<TilledWindow*>::iterator     jt;

        // foreach it in windowList
        for ( it = windowList.begin()
            ; it != windowList.end()
            ; ++it )
        {
            for ( jt = it->windowList.begin()
                ; jt != it->windowList.end()
                ; ++jt )
            {
                delete *jt;
            }
        }
    }
}
