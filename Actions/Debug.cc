#include <fstream>
#include "Debug.h"

namespace ViWm {
namespace Actions
{
    GraphVizDump::GraphVizDump( StringId display
                            , StringId descr
                            , const char* dumpfilename)
        : Action( display, descr )
        , filename( dumpfilename )
    {}

    struct GraphVizOutputer
    {
        std::ostream&  what;
        GraphVizOutputer( std::ostream& nwhat ) : what( nwhat ) {}
        
        bool    operator() ( LayoutNode& father, LayoutNode::SizePair &p )
        {
            if ( p.subTree == NULL )
                what << (int)&p << " [label=\"NULL\"];\n";

            what << (int)&father
                 << " -> ";

            if ( p.subTree == NULL )
                what << (int)&p;
            else
                what << (int)p.subTree;

            what << ";\n";
            return false;
        }
    private:
        void    operator = (const GraphVizOutputer&);
    };

    Action::ReturnInfo GraphVizDump::operator()( DesktopLayout &l
                                               , WindowMakerState &state )
    {
        std::ofstream       out( filename );
        GraphVizOutputer    gout( out );
        LayoutNode::IteratingPredicate  pred( gout );

        out << "digraph debug\n"
            << "{\n";

        for (size_t i = 0; i < l.size(); i++)
        {
            LayoutNode* node = dynamic_cast<LayoutNode*>( l[i].layoutRoot );
            if ( node )
                node->Iter( pred );
        }

        out << "}\n";

        out.close();

        return Nothing;
    }
}}
