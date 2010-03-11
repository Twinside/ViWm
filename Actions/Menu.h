#ifndef __MENU_H__
#define __MENU_H__

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class ActionMenu : public Action
    {
    public:
        ActionMenu( LayoutTree::WindowKey   receiver
                  , const HotKeyCollection  &actions );
        ~ActionMenu();
        
        virtual ReturnInfo  operator ()( DesktopLayout& l
                                       , WindowMakerState &s );

    private:
        void operator =( const ActionMenu & );

        void    addToMenu( HMENU menu, int position, int index, TCHAR* str );
        void    populateMenu( HMENU menu );

        const HotKeyCollection& commands;
        HWND                       receivingWindow;
    };
}}

#endif /* __MENU_H__ */
