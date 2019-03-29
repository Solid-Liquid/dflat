//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "lexer.hpp"

using namespace dflat;

namespace dflat
{

TEST_CASE( "Code generator works correctly", "[generator]" )
{
    // Please forgive insane formatting :)
    // == denotes the comparison between input and expected output
    // PT[test function, token input...] == ~[resulting exp, block, or stm]


    /*
     * Tests for success on individual functions:
     */

//    REQUIRE( PT(parseNumber, //6 -> NumberExp
//        NumberToken(6)
//        )
//        ==
//        ~NumberExp(6)
//        );
}
}
