//Unit tests for the parser

#include "catch2/catch.hpp"
#include "parser.hpp"
#include "token_helpers.hpp"
#include <iostream>

using namespace dflat;

// Convenience function for making ASN trees to test against.
template <typename T>
ASNPtr operator~(T&& t)
{
    return std::make_unique<T>(std::forward<T>(t));
}



//Parser( tokens(NumberToken(1), PlusToken(), NumberToken(1)) ).parseAdditive()
#define PT(method, ...) Parser(tokens(__VA_ARGS__)).method()

TEST_CASE( "Parser works correctly", "[parser]" )
{
    // Please forgive insane formatting :)
    // == denotes the comparison between input and expected output
    // PT[test function, token input...] == ~[resulting exp, block, or stm]


    /*
     * Tests for success on individual functions:
     */

    REQUIRE( PT(parseNumber, //6 -> NumberExp
        NumberToken(6)
        )
        ==
        ~NumberExp(6)
        );

    REQUIRE( PT(parseExp, //same as above but using parseExp
        NumberToken(6)
        )
        ==
        ~NumberExp(6)
        );

    REQUIRE( PT(parseVariable, //fun -> VariableExp
        VariableToken("fun")
        )
        ==
        ~VariableExp("fun")
        );

    REQUIRE( PT(parseExp,      //same as above but using parseExp
        VariableToken("fun")
        )
        ==
        ~VariableExp("fun")
        );
    
    REQUIRE( PT(parseAdditive,  //1 + 1 -> BinopExp(additive)
        NumberToken(1),
        PlusToken(),
        NumberToken(1)
        )
        ==
        ~BinopExp(
            ~NumberExp(1),
            opPlus,
            ~NumberExp(1)
            )
        );

    REQUIRE( PT(parseAdditive, //2 - 5 -> BinopExp(additive)
        NumberToken(2),
        MinusToken(),
        NumberToken(5)
        )
        ==
        ~BinopExp(
            ~NumberExp(2),
            opMinus,
            ~NumberExp(5)
            )
        );

    REQUIRE( PT(parseExp, //same as above but using parseExp
        NumberToken(2),
        MinusToken(),
        NumberToken(5)
        )
        ==
        ~BinopExp(
            ~NumberExp(2),
            opMinus,
            ~NumberExp(5)
            )
        );

    REQUIRE( PT(parseMultive,     //2 * 3 -> BinopExp(multive)
                NumberToken(2),
                MultiplyToken(),
                NumberToken(3)
                )
             ==
             ~BinopExp(
                ~NumberExp(2),
                opMult,
                ~NumberExp(3)
                 )
             );

    REQUIRE( PT(parseMultive,     //10 / 5 -> BinopExp(multive)
                NumberToken(10),
                DivisionToken(),
                NumberToken(5)
                )
             ==
             ~BinopExp(
                ~NumberExp(10),
                opDiv,
                ~NumberExp(5)
                 )
             );

    REQUIRE( PT(parseExp,         //same as above but using parseExp
                NumberToken(10),
                DivisionToken(),
                NumberToken(5)
                )
             ==
             ~BinopExp(
                ~NumberExp(10),
                opDiv,
                ~NumberExp(5)
                 )
             );

    REQUIRE( PT(parseLogical,         //foo && bar -> BinopExp(logical)
                VariableToken("foo"),
                AndToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opAnd,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseLogical,         //foo || bar -> BinopExp(logical)
                VariableToken("foo"),
                OrToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opOr,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseLogical,         //same as above but using parseExp
                VariableToken("foo"),
                OrToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opOr,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseUnary,    //-1 -> UnaryExp
                MinusToken(),
                NumberToken(1)
                )
             ==
             ~UnopExp(~NumberExp(1), opMinus)
            );

    REQUIRE( PT(parseUnary,    //!var -> UnaryExp
                NotToken(),
                VariableToken("var")
                )
             ==
             ~UnopExp(~VariableExp("var"), opNot)
            );

    REQUIRE( PT(parseExp,    //same as above but using parseExp
                NotToken(),
                VariableToken("var")
                )
             ==
             ~UnopExp(~VariableExp("var"), opNot)
            );

    REQUIRE( PT(parseExp,  //BinopExp(additive) with nested BinopExp(multive)
        NumberToken(1),    // 1 + 1 * 4
        PlusToken(),
        NumberToken(1),
        MultiplyToken(),
        NumberToken(4)
        )
        ==
        ~BinopExp(
            ~NumberExp(1),
            opPlus,
            ~BinopExp(
                ~NumberExp(1),
                opMult,
                ~NumberExp(4)
                )
            )
        );

    REQUIRE( PT(parseBlock,
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~Block()
             );

    REQUIRE( PT(parseIfStmt,
                IfToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~IfStm(~NumberExp(1),
                      ~Block(),
                      ~Block())
             );

// new test
    REQUIRE( PT(parseNew,
                NewToken(),
                VariableToken("int"),
                LeftParenToken(),
                RightParenToken()
                )
             ==
             ~NewExp("int",
                      Vector<ASNPtr>{})
             );

// new test with 1 expression
    REQUIRE( PT(parseNew,
                NewToken(),
                VariableToken("int"),
                LeftParenToken(),
                NumberToken(3),
                RightParenToken()
                )
             ==
             ~NewExp("int",
                      Vector<ASNPtr>{~NumberExp(3)})
             );
    /*
     * nullptr is properly returned for unsuccessful parse:
     */

    REQUIRE( PT(parseNumber,  //parse is not Number
        VariableToken("var")
        )
        ==
        nullptr
        );

    REQUIRE( PT(parseVariable, //parse is not Variable
        NumberToken(6)
        )
        ==
        nullptr
        );

    REQUIRE( PT(parseAdditive,      //parse is not Additive
                NumberToken(2)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseAdditive,      //parse is not Additive
                NumberToken(2),
                MultiplyToken(),
                NumberToken(3)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseMultive,      //parse is not Multive
                NumberToken(2),
                PlusToken(),
                NumberToken(3)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseUnary,           //parse is not Unary
                VariableToken("var")
                )
             ==
             nullptr
            );
}
