//Unit tests for the lexer

#include "catch2/catch.hpp"
#include "token_helpers.hpp"
#include "lexer.hpp"

using namespace dflat;

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
            case tokComma:
            case tokSemi:
            case tokWhile:
            case tokAnd:
            case tokOr:
            case tokEq:
            case tokNotEq:
            case tokNot:
            case tokMember:
            case tokNew:
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

    REQUIRE ( tokenize(";") == tokens(
        SemiToken()
        ));

    REQUIRE ( tokenize("if") == tokens(
        IfToken()
        ));

    REQUIRE ( tokenize("else") == tokens(
        ElseToken()
        ));

    REQUIRE ( tokenize("new") == tokens(
        NewToken()
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

    REQUIRE ( tokenize(",") == tokens(
        CommaToken()
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
    
    REQUIRE ( tokenize("*if+else-/while") == tokens(
        MultiplyToken(),
        IfToken(),
        PlusToken(),
        ElseToken(),
        MinusToken(),
        DivisionToken(),
        WhileToken()
        ));

    //Tests for bad input:

    REQUIRE_THROWS_AS( tokenize("$"), LexerException );

    REQUIRE_THROWS_AS( tokenize("@"), LexerException );

    REQUIRE_THROWS_AS( tokenize("x = 2 + $"), LexerException );

    REQUIRE_THROWS_AS( tokenize("x @ y"), LexerException );

    REQUIRE_THROWS_AS( tokenize("var$ == 56"), LexerException );
}
