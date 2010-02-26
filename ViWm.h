#ifndef VIWM_H
#define VIWM_H

#include <vector>
#include <memory>
#include "TilledWindow.h"
#include "Screen.h"
#include "State.h"

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

        void    AddNode( HWND hwnd );
        void    ArrangeWindows();
        void    FocusCurrent();

        LRESULT HandleShellHook( HWND hwnd, UINT msg
                               , WPARAM wParam, LPARAM lParam );
        void    HandleHotKey( WPARAM wParam );

    private:
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

        typedef std::tr1::shared_ptr<Layouter>
                LayoutPtr;

        //////////////////////////////////////////////////////////////////////////
        ////              Our data!
        //////////////////////////////////////////////////////////////////////////
        HotKeyCollection        hotkeysDefinition;
        std::vector<LayoutPtr>  layouter;

        /**
         * Modkey used to define global hotkey.
         */
        int modkeys;
        
        Renderer::Renderer      defaultRenderer;

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
