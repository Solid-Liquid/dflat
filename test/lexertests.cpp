//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "lexer.hpp"

using namespace dflat;

TEST_CASE( "LexerCore handles input correctly", "[lexercore]" )
{
    LexerCore lc("ab");

    // Initial state.
    REQUIRE ( lc._input == "ab" );
    REQUIRE ( lc._pos   == 0 );
    REQUIRE ( lc.peek() == 'a' );
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

// Convenience function for making Token vectors to test against.
template <typename... Ts>
Vector<TokenPtr> tokens(Ts&&... in)
{
    Vector<TokenPtr> out;
    (out.push_back(std::make_unique<Ts>(in)), ...);
    return out;
}

TEST_CASE( "Lexer produces correct output", "[lexer]" )
{
    REQUIRE ( tokenize("3") == tokens(NumberToken(3)) );
    //TODO more tests
}
