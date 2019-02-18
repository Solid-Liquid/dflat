//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "lexercore.hpp"

using namespace dflat;

//Temporary test just to prove that the unit test library works!
unsigned int Factorial( unsigned int number )
{
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" )
{
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE( "LexerCore handles input correctly", "[lexercore]" )
{
    LexerCore lc("ab");

    // Initial state.
    REQUIRE ( lc._input == "ab" );
    REQUIRE ( lc._pos   == 0 );
    REQUIRE ( lc.peek() == 'a' );

    // Advance 1 and check.
    REQUIRE ( lc.get()  == 'a' );
    REQUIRE ( lc.peek() == 'b' );
    REQUIRE ( lc._pos   == 1 );

    // Advance 1 and check.
    REQUIRE ( lc.get()  == 'b' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2);

    // Advancing when at end should be idempotent.
    REQUIRE ( lc.get()  == '\0' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2);
    
    REQUIRE ( lc.get()  == '\0' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2);
}
