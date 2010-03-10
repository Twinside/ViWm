#include "Menu.h"
#include <tchar.h>

namespace ViWm {
namespace Actions
{
    ActionMenu::ActionMenu( LayoutTree::WindowKey   receiver
                          , const HotKeyCollection& actions )
        : Action( NoStringId, NoStringId )
        , commands( actions )
        , receivingWindow( receiver )
    {}

    ActionMenu::~ActionMenu()
    {}

    TCHAR   str[MAX_PATH];
    void ActionMenu::populateMenu( HMENU menu )
    {
        HINSTANCE   hInst = GetModuleHandle( NULL );

        for ( size_t i = 0; i < commands.size(); i++ )
        {
            Action *act = commands[i].second;
            if ( act != this && act->getDisplayName() != NoStringId)
            {
                LoadString( hInst, act->getDisplayName(), str
                          , sizeof( str ) / sizeof( TCHAR ) );
                addToMenu( menu, i,  str );
            }
        }
    }

    void ActionMenu::addToMenu( HMENU menu, int index, TCHAR* str )
    {
        if (AppendMenu( menu, MF_STRING, index, str ) == 0)
        {
            int errCode = GetLastError();
            MessageBox( NULL
                      , TEXT("Error while creating menu item")
                      , TEXT("Menu creation error")
                      , MB_OK );
            exit( 1 );
        }
    }

    Action::ReturnInfo ActionMenu::operator()( DesktopLayout& l, WindowMakerState &s )
    {
        HMENU                      menu;
        POINT pt;

        menu = CreatePopupMenu();
        populateMenu( menu );

        SetForegroundWindow( receivingWindow );
        GetCursorPos( &pt );
        int i = 19;
        AppendMenu( menu, MF_STRING, i++, _TEXT("mouais mouais mouais") );
        AppendMenu( menu, MF_STRING, i++, _TEXT("mouais mouais") );
        AppendMenu( menu, MF_STRING, i++, _TEXT("mouais ") );
        AppendMenu( menu, MF_STRING, i++, _TEXT("mouais mouais joaiejfoia mouais") );
        TrackPopupMenuEx( menu
                        , 0
                        , pt.x, pt.y
                        , receivingWindow
                        , NULL );

        // required to make menu show and hide correctly
        PostMessage( receivingWindow, WM_NULL, 0, 0 );
        DestroyMenu( menu );

        return Nothing;
    }
}}
