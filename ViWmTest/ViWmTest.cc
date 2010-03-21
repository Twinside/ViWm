#include <iostream>

#include "../Constants.h"
#include "../WinTypeTestSet.h"
#   include "../Actions/TreeAction.h"
#   include "../Actions/MultiScreen.h"
#   include "../Actions/System.h"
#   include "../Actions/Debug.h"
#   include "../Rendering/RenderWindow.h"
#include "../WinTypeTestUnset.h"
#include "TestTreeBuilder.h"

#define BOOST_TEST_MODULE "ViWm Action no crash test"

#define BOOST_TEST_MAIN "ViWm Test Suite"
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>

//#include <boost/test/test_tools.hpp>
//#include <boost/test/unit_test_suite.hpp>

using namespace ViWm;

BOOST_AUTO_TEST_SUITE( singleWindow )

/**
 * Each instance of this class produce all the required command
 * for the tests.
 */
struct CommandBuilder 
{
    CommandBuilder()
    {
        actions.push_back( new Actions::GraphVizDump( NoStringId, NoStringId, "dump.dot" ));
        actions.push_back( new Actions::VerticalSplit( NoStringId, NoStringId ) );
        actions.push_back( new Actions::HorizontalSplit( NoStringId, NoStringId ) );
        actions.push_back( new Actions::NodeRotate( NoStringId, NoStringId ,1 ) );
        actions.push_back( new Actions::InverseScreenLayout( NoStringId, NoStringId ));
        actions.push_back( new Actions::Relayout( NoStringId, NoStringId ));

        actions.push_back( new Actions::HorizontalMove( NoStringId, NoStringId, -1 ));
        actions.push_back( new Actions::VerticalMove( NoStringId, NoStringId, 1 ));
        actions.push_back( new Actions::VerticalMove( NoStringId, NoStringId,  -1 ));
        actions.push_back( new Actions::HorizontalMove( NoStringId, NoStringId,  1 ));

        actions.push_back( new Actions::ScreenMoverBounded( NoStringId, NoStringId ,-1 ));
        actions.push_back( new Actions::ScreenMoverBounded( NoStringId, NoStringId, 1 ));
    }

    ~CommandBuilder()
    {
        for (size_t i = 0; i < actions.size(); i++)
            delete actions[i];
    }
    std::vector<Action*> actions;
};

struct SimpleWindowSetup 
{
    SimpleWindowSetup()
        : conf( "testConf.txt" )
        , state( conf )
    {
        // SETUP
        layout.push_back( *new Screen( *new Renderer::RenderWindow( 0, 0, 0, 0, 0 )
                                     , 0, 0, 800, 600 ));
        state.addWindow( *new TilledWindow( 1 ));
    }

    ~SimpleWindowSetup()
    {
        for ( size_t i = 0; i < layout.size(); i++ )
            delete &layout[i];
    }
    
    Configuration       conf;
    WindowMakerState    state;
    DesktopLayout       layout;
    // TO BE PUT in shit
};

//////////////////////////////////////////////////////////////////////////
////                1D
//////////////////////////////////////////////////////////////////////////
#define UNI_TEST(n,fixture) \
    BOOST_FIXTURE_TEST_CASE( simple_test##n , fixture) \
    { \
        CommandBuilder  b; \
        (*b.actions[(n)])( layout, state ); \
    }

#define UNI_RANGE(fixture) \
        UNI_TEST(0, fixture) \
        UNI_TEST(1, fixture) \
        UNI_TEST(2, fixture) \
        UNI_TEST(3, fixture) \
        UNI_TEST(4, fixture) \
        UNI_TEST(5, fixture) \
        UNI_TEST(6, fixture) \
        UNI_TEST(7, fixture) \
        UNI_TEST(8, fixture) \
        UNI_TEST(9, fixture) \
        UNI_TEST(10, fixture) \
        UNI_TEST(11, fixture)

//////////////////////////////////////////////////////////////////////////
////                2D
//////////////////////////////////////////////////////////////////////////
#define BI_TEST(name,n,m,fixture) \
    BOOST_FIXTURE_TEST_CASE( name, fixture) \
    { \
        CommandBuilder  b; \
        (*b.actions[(n)])( layout, state ); \
        (*b.actions[(m)])( layout, state ); \
    }

#define BI_LINE(name, n,fixture) \
        BI_TEST( Test0_##name, n, 0, fixture ) \
        BI_TEST( Test1_##name,n,1, fixture) \
        BI_TEST( Test2_##name,n,2, fixture) \
        BI_TEST( Test3_##name,n,3, fixture) \
        BI_TEST( Test4_##name,n,4, fixture) \
        BI_TEST( Test5_##name,n,5, fixture) \
        BI_TEST( Test6_##name,n,6, fixture) \
        BI_TEST( Test7_##name,n,7, fixture) \
        BI_TEST( Test8_##name,n,8, fixture) \
        BI_TEST( Test9_##name,n,9, fixture) \
        BI_TEST( Test10_##name,n,10, fixture) \
        BI_TEST( Test11_##name,n,11, fixture)


#define BI_RANGE(name,fixture) \
        BI_LINE( 0_##name, 0, fixture ) \
        BI_LINE( 1_##name, 1, fixture ) \
        BI_LINE( 2_##name, 2, fixture ) \
        BI_LINE( 3_##name, 3, fixture ) \
        BI_LINE( 4_##name, 4, fixture ) \
        BI_LINE( 5_##name, 5, fixture ) \
        BI_LINE( 6_##name, 6, fixture ) \
        BI_LINE( 7_##name, 7, fixture ) \
        BI_LINE( 8_##name, 8, fixture ) \
        BI_LINE( 9_##name, 9, fixture ) \
        BI_LINE( 10_##name, 10, fixture ) \
        BI_LINE( 11_##name, 11, fixture )

//////////////////////////////////////////////////////////////////////////
////                3D
//////////////////////////////////////////////////////////////////////////
#define TRI_TEST(name,n,m,o,fixture) \
    BOOST_FIXTURE_TEST_CASE( name, fixture) \
    { \
        CommandBuilder  b; \
        (*b.actions[(n)])( layout, state ); \
        (*b.actions[(m)])( layout, state ); \
        (*b.actions[(o)])( layout, state ); \
    }

#define TRI_LINE(name,n,m,fixture) \
        TRI_TEST(Test0_##name , n, m, 0, fixture) \
        TRI_TEST(Test1_##name , n, m, 1, fixture ) \
        TRI_TEST(Test2_##name , n, m, 2, fixture ) \
        TRI_TEST(Test3_##name , n, m, 3, fixture ) \
        TRI_TEST(Test4_##name , n, m, 4, fixture ) \
        TRI_TEST(Test5_##name , n, m, 5, fixture ) \
        TRI_TEST(Test6_##name , n, m, 6, fixture ) \
        TRI_TEST(Test7_##name , n, m, 7, fixture ) \
        TRI_TEST(Test8_##name , n, m, 8, fixture ) \
        TRI_TEST(Test9_##name , n, m, 9, fixture ) \
        TRI_TEST(Test10_##name , n, m, 10, fixture ) \
        TRI_TEST(Test11_##name , n, m, 11, fixture )

#define TRI_PLANE(name, n,fixture) \
        TRI_LINE( 0_##name,n, 0, fixture ) \
        TRI_LINE( 1_##name,n,1, fixture) \
        TRI_LINE( 2_##name,n,2, fixture) \
        TRI_LINE( 3_##name,n,3, fixture) \
        TRI_LINE( 4_##name,n,4, fixture) \
        TRI_LINE( 5_##name,n,5, fixture) \
        TRI_LINE( 6_##name,n,6, fixture) \
        TRI_LINE( 7_##name,n,7, fixture) \
        TRI_LINE( 8_##name,n,8, fixture) \
        TRI_LINE( 9_##name,n,9, fixture) \
        TRI_LINE( 10_##name,n,10, fixture) \
        TRI_LINE( 11_##name,n,11, fixture)

#define TRI_RANGE(name,fixture) \
        TRI_PLANE( 0_##name, 0, fixture ) \
        TRI_PLANE( 1_##name, 1, fixture ) \
        TRI_PLANE( 2_##name, 2, fixture ) \
        TRI_PLANE( 3_##name, 3, fixture ) \
        TRI_PLANE( 4_##name, 4, fixture ) \
        TRI_PLANE( 5_##name, 5, fixture ) \
        TRI_PLANE( 6_##name, 6, fixture ) \
        TRI_PLANE( 7_##name, 7, fixture ) \
        TRI_PLANE( 8_##name, 8, fixture ) \
        TRI_PLANE( 9_##name, 9, fixture ) \
        TRI_PLANE( 10_##name, 10, fixture ) \
        TRI_PLANE( 11_##name, 11, fixture )

// Tests
UNI_RANGE( SimpleWindowSetup )
BI_RANGE( simple, SimpleWindowSetup )
//TRI_RANGE( simple, SimpleWindowSetup )

BOOST_AUTO_TEST_SUITE_END()
