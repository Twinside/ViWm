#include <iostream>

#include "../Constants.h"
#include "../WinTypeTestSet.h"
#   include "../Actions/TreeAction.h"
#   include "../Actions/MultiScreen.h"
#   include "../Actions/System.h"
#   include "../Actions/Debug.h"
#include "../WinTypeTestUnset.h"

#define BOOST_TEST_MODULE "ViWm Action no crash test"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE( singleWindow )

struct SimpleWindowSetup 
{
    SimpleWindowSetup()
    {
        // SETUP
    }

    ~SimpleWindowSetup()
    {
        // TEARDOWN
    }
    
    // TO BE PUT in shit
};

BOOST_AUTO_TEST_SUITE_END()
