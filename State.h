#ifndef STATE_H
#define STATE_H

#include <list>
#include <vector>
#include "Constants.h"
#include "Layouter.h"

namespace ViWm
{
    class TilledWindow;

    class WindowMakerState
    {
    public:
        WindowMakerState();
        ~WindowMakerState();
        
        enum Configuration
        {
            // max recursion for layout tree
            // 10 is a reasonable value, MaxRecursion,
            // is more than enough.
            MaxRecursion = 128
        };

        TilledWindow*   FindNode( HWND hwnd );
        void            RemoveNode( HWND hwnd );

        int alpha;

        TillingMode tilingMode;

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
    };
}

#endif /* STATE_H */