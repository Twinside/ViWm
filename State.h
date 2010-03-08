#ifndef STATE_H
#define STATE_H

#include <list>
#include <vector>
#include <memory>
#include "Constants.h"
#include "Layouter.h"

namespace ViWm
{
    class TilledWindow;
    class Configuration;

    class WindowMakerState
    {
    public:
        WindowMakerState( const Configuration &conf );
        ~WindowMakerState();
        
        enum StateConfiguration
        {
            // max recursion for layout tree
            // 10 is a reasonable value, MaxRecursion,
            // is more than enough.
            MaxRecursion = 128
        };

        TilledWindow*   FindNode( HWND hwnd );
        void            RemoveNode( HWND hwnd );

        int alpha;

        TillingMode getTillingMode() { return tilingMode; }
        Layouter&   getCurrentLayouter()
            { return *layouter[tilingMode]; }

        /* Should always point to current node */
        TilledWindow *current; 	
        size_t       currentTag;
        size_t       currentScreen;

        struct Bucket
        {
            int bucketCurrent;
            std::list< TilledWindow* >  windowList;
        };

        /**
         * One window list per tag.
         */
        std::vector<Bucket> windowList;

    private:
        TillingMode tilingMode;

        typedef std::tr1::shared_ptr<Layouter>
                LayoutPtr;

        std::vector<LayoutPtr>  layouter;
    };
}

#endif /* STATE_H */