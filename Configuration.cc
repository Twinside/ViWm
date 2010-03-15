#include "Configuration.h"

namespace ViWm
{
    Configuration::Configuration(const char* /*filename*/)
        : equalAlways( true )
        , alpha( 200 )
    {}

    Configuration::~Configuration()
    {}
}

