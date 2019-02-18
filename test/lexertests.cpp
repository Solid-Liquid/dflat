//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "lexercore.hpp"

using namespace dflat;

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
