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
    //Tests for single tokens getting tokenized properly:

    REQUIRE ( tokenize("3") == tokens(
        NumberToken(3)
        ));

    REQUIRE ( tokenize("450") == tokens(
        NumberToken(450)
        ));

    REQUIRE ( tokenize("x") == tokens(
        VariableToken("x")
        ));

    REQUIRE ( tokenize("func") == tokens(
        VariableToken("func")
        ));

    REQUIRE ( tokenize("\n") == tokens(
        NewlineToken()
        ));

    REQUIRE ( tokenize("if") == tokens(
        IfToken()
        ));

    REQUIRE ( tokenize("else") == tokens(
        ElseToken()
        ));

    REQUIRE ( tokenize("+") == tokens(
        PlusToken()
        ));

    REQUIRE ( tokenize("-") == tokens(
        MinusToken()
        ));

    REQUIRE ( tokenize("/") == tokens(
        DivisionToken()
        ));

    REQUIRE ( tokenize("*") == tokens(
        MultiplyToken()
        ));

    REQUIRE ( tokenize("=") == tokens(
        EqualToken()
        ));

    REQUIRE ( tokenize("{") == tokens(
        LeftBraceToken()
        ));

    REQUIRE ( tokenize("}") == tokens(
        RightBraceToken()
        ));

    REQUIRE ( tokenize("(") == tokens(
        LeftParenToken()
        ));

    REQUIRE ( tokenize(")") == tokens(
        RightParenToken()
        ));

    REQUIRE ( tokenize("for") == tokens(
        ForToken()
        ));

    REQUIRE ( tokenize("while") == tokens(
        WhileToken()
        ));

    REQUIRE ( tokenize("!") == tokens(
        NotToken()
        ));

    REQUIRE ( tokenize("||") == tokens(
        LogicalToken()
        ));

    REQUIRE ( tokenize("&&") == tokens(
        LogicalToken()
        ));

    //Tests for multiple tokens and special cases being tokenized:
    
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
