#ifndef VIWM_H
#define VIWM_H

#include <vector>
#include <set>
#include "TilledWindow.h"
#include "Screen.h"
#include "State.h"
#include "Configuration.h"

namespace ViWm
{
    class Action;
    class Layouter;

    typedef int VirtualKey;
    typedef std::pair<VirtualKey, Action*> HotKey;
    typedef std::vector<HotKey> HotKeyCollection;

    class ViWmManager
    {
    public:
        ViWmManager( HINSTANCE hInstance
            , const HotKeyCollection  &originalConfiguration
            );
        ~ViWmManager();

        void    Init();

        /**
         * Add a window to be managed.
         */
        void    AddNode( HWND hwnd );

        /**
         * Replace the windows.
         */
        void    ArrangeWindows();

        /**
         * Give the focus to the current window.
         * The window can be on any screens.
         */
        void    FocusCurrent();

        /**
         * In charge of listening to hotkey actions.
         */
        LRESULT HandleShellHook( HWND hwnd, UINT msg
                               , WPARAM wParam, LPARAM lParam );

        /**
         * Method to call when handling a recognized hotkey.
         * Will call the corresponding action.
         */
        void    HandleHotKey( WPARAM wParam );

        /**
         * Retrieve the window which receive hotkey messages.
         * You can post messages to this window if you want.
         */
        LayoutTree::WindowKey GetReceivingWindow()
            { return globalHotkeyListener; }

        void                  AddHotkey( HotKey k );


        void    beginPick( int x, int y );
        void    movePick( int x, int y );
        void    endPick();

        bool    inDrag() { return currentSplitMove.first != NULL; }
        LayoutTree::SplitSide   QuerySplitDirection( int x, int y );

    private:
        Configuration   conf;

        //////////////////////////////////////////////////////////////////////////
        ////                    Just to avoid compiler warning
        //////////////////////////////////////////////////////////////////////////
        void    operator =( const ViWmManager& );
        ViWmManager( const ViWmManager & );


        //////////////////////////////////////////////////////////////////////////
        ////              Function hidden to the rest of the world
        //////////////////////////////////////////////////////////////////////////
        void    UnregisterHotKeys( HWND hwnd );
        void    RegisterHotKeys( HWND hwnd );
        void    createGlobalListener( HINSTANCE hInstance );
        void    selectWindow( HWND currentWindow );

        //////////////////////////////////////////////////////////////////////////
        ////              Static Win32 Func
        //////////////////////////////////////////////////////////////////////////
        // static functions to be used with Win32
        static LRESULT CALLBACK keyListenerProc( HWND hwnd
                                               , UINT msg
                                               , WPARAM wParam
                                               , LPARAM lParam );
        static LRESULT CALLBACK windowListenerProc( HWND hwnd
                                                  , UINT msg
                                                  , WPARAM wParam
                                                  , LPARAM lParam );

        static BOOL CALLBACK
                 monitorEnumerator( HMONITOR hMonitor
                                  , HDC      hdcMonitor
                                  , LPRECT   intersectionRect
                                  , LPARAM   userData );

        //////////////////////////////////////////////////////////////////////////
        ////              Our data!
        //////////////////////////////////////////////////////////////////////////
        HotKeyCollection        hotkeysDefinition;

        /**
         * Modkey used to define global hotkey.
         */
        int modkeys;
        LayoutTree::SplitCoord  currentSplitMove;
        size_t                  currentScreenMove;

        HINSTANCE               hInstance;
        DesktopLayout           currentLayout;
        WindowMakerState        currentState;
        HWND                    globalHotkeyListener;
        UINT                    shellhookid;	/* Window Message id */

        /*
        // Don't include these classes in tiling
        char ignoreClasses[MAX_IGNORE][128];		
        // Don't remove borders from these classes
        char ignoreClassesBorders[MAX_IGNORE][128]; //*/
    };

}

#endif /* VIWM_H */
