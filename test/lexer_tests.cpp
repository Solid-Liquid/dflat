//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "token_helpers.hpp"
#include "lexer.hpp"

using namespace dflat;

// TODO This is hardly ideal
namespace dflat 
{
    template <typename T>
    bool cmp(TokenPtr const&, TokenPtr const&) = delete;

    template <>
    bool cmp<NumberToken>(TokenPtr const& a, TokenPtr const& b)
    {
        return a->as<NumberToken>()->num
            == b->as<NumberToken>()->num;
    }

    template <>
    bool cmp<VariableToken>(TokenPtr const& a, TokenPtr const& b)
    {
        return a->as<VariableToken>()->name
            == b->as<VariableToken>()->name;
    }

    bool operator==(TokenPtr const& a, TokenPtr const& b)
    {
        if (a->getType() != b->getType())
        {
            return false;
        }

        switch (a->getType())
        {
            case tokNum: 
                return cmp<NumberToken>(a, b);
            
            case tokVar: 
                return cmp<VariableToken>(a, b);
            
            case tokIf:
            case tokElse:
            case tokPlus:
            case tokMinus:
            case tokDiv:
            case tokAssign:
            case tokMult:
            case tokRBrace:
            case tokLBrace:
            case tokLParen:
            case tokRParen:
            case tokNewLine:
            case tokFor:
            case tokWhile:
            case tokAnd:
            case tokOr:
            case tokEq:
            case tokNotEq:
            case tokNot:
            case tokMember:
                 return true;
     
            default: 
                std::abort(); 
                return false;
        }
    }
}

TEST_CASE( "Lexer produces correct output", "[lexer]" )
{
    //Tests for single tokens getting tokenized properly:
    
    REQUIRE ( tokens(NumberToken(1)) == tokens(
        NumberToken(1)
        ));

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
        AssignToken()
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
        OrToken()
        ));

    REQUIRE ( tokenize("&&") == tokens(
        AndToken()
        ));
    
    REQUIRE ( tokenize("==") == tokens(
        EqToken()
        ));
    
    REQUIRE ( tokenize("!=") == tokens(
        NotEqToken()
        ));

    REQUIRE ( tokenize(".") == tokens(
        MemberToken()
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
    
    REQUIRE ( tokenize("===") == tokens(
        EqToken(), 
        AssignToken()
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
