//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "lexer.hpp"

using namespace dflat;

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
    REQUIRE ( tokenize("3") == tokens(
        NumberToken(3)
        ));
    
    REQUIRE ( tokenize("3    ") == tokens(
        NumberToken(3)
        ));
    
    REQUIRE ( tokenize("    3") == tokens(
        NumberToken(3)
        ));

    REQUIRE ( tokenize("hello") == tokens( 
        VariableToken("hello") 
        ));
    
    REQUIRE ( tokenize(" 3 x ") == tokens( 
        NumberToken(3),
        VariableToken("x")
        )); 
    
    REQUIRE ( tokenize("3x") == tokens( 
        NumberToken(3),
        VariableToken("x")
        )); 
    
    REQUIRE ( tokenize("x3") == tokens( 
        VariableToken("x3")
        )); 
    
    REQUIRE ( tokenize("({})") == tokens(
        LeftParenToken(), 
        LeftBraceToken(),
        RightBraceToken(),
        RightParenToken()
        ));
    
    REQUIRE ( tokenize("*if+else-for/while") == tokens(
        MultiplyToken(),
        IfToken(),
        PlusToken(),
        ElseToken(),
        MinusToken(),
        ForToken(),
        DivisionToken(),
        WhileToken()
        ));

    //TODO more tests
}
