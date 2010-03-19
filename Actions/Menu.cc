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

    void ActionMenu::populateMenu( HMENU menu )
    {
        TCHAR   str[MAX_PATH];
        HINSTANCE   hInst = GetModuleHandle( NULL );

        for ( size_t i = 0; i < commands.size(); i++ )
        {
            Action *act = commands[i].second;
            if ( act != this && act->getDisplayName() != NoStringId)
            {
                LoadString( hInst, act->getDisplayName(), str
                          , sizeof( str ) / sizeof( TCHAR ) );
                addToMenu( menu, str );
            }
        }
    }

    void ActionMenu::addToMenu( HMENU menu, TCHAR* str )
    {
        if (AppendMenu( menu, MF_STRING, NULL, str ) == 0)
        {
            //int errCode = GetLastError();
            MessageBox( NULL
                      , TEXT("Error while creating menu item")
                      , TEXT("Menu creation error")
                      , MB_OK );
            exit( 1 );
        }
    }

    Action::ReturnInfo ActionMenu::operator()( DesktopLayout& /*l*/, WindowMakerState &/*s*/ )
    {
        HMENU                      menu;
        POINT pt;

        menu = CreatePopupMenu();
        populateMenu( menu );

        //SetMenu( receivingWindow, menu );
        SetForegroundWindow( receivingWindow );
        GetCursorPos( &pt );

        DrawMenuBar( receivingWindow );
        BOOL good =
            TrackPopupMenuEx( menu
                            , TPM_RIGHTALIGN
                            , pt.x, pt.y
                            , receivingWindow
                            , NULL );

        if ( good == FALSE )
        {
            //int err = GetLastError();
            throw;
        }
        DestroyMenu( menu );

        return Nothing;
    }
}}
