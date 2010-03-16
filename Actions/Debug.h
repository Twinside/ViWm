#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "../Action.h"

namespace ViWm {
namespace Actions
{
    class GraphVizDump : public Action
    {
    public:
        GraphVizDump( StringId display
                    , StringId descr
                    , const char* dumpfilename );
        
        ReturnInfo operator()( DesktopLayout &l
                             , WindowMakerState &state );

    private:
        const char* filename;
    };
}}

#endif /* __DEBUG_H__ */

