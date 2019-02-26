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
    REQUIRE ( lc.peek_ahead(0) == 'a' );
    REQUIRE ( lc.peek_ahead(1) == 'b' );
    REQUIRE ( lc.peek_ahead(2) == '\0' );
    REQUIRE ( lc.peek_ahead(3) == '\0' );
    REQUIRE ( lc.at_end() == false );

    // Advance 1 and check.
    REQUIRE ( lc.get()  == 'a' );
    REQUIRE ( lc.peek() == 'b' );
    REQUIRE ( lc._pos   == 1 );
    REQUIRE ( lc.at_end() == false );

    // Advance 1 and check.
    REQUIRE ( lc.get()  == 'b' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2 );
    REQUIRE ( lc.at_end() == true );

    // Advancing when at end should be idempotent.
    REQUIRE ( lc.get()  == '\0' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2);
    REQUIRE ( lc.at_end() == true );
    
    REQUIRE ( lc.get()  == '\0' );
    REQUIRE ( lc.peek() == '\0' );
    REQUIRE ( lc._pos   == 2);
    REQUIRE ( lc.at_end() == true );
}
