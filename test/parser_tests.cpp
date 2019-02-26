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
//    REQUIRE( PT(parseIfStmt,
//                IfToken(),
//                LeftParenToken(),
//                NumberToken(1),
//                RightParenToken(),
//                LeftBraceToken(),
//                RightBraceToken(),
//                ElseToken(),
//                LeftBraceToken(),
//                RightBraceToken()
//                ) ==
//             ~IfBlock(~NumberExp(1),
//                      ~Block(),
//                      ~Block())
//             );

    // Please forgive insane formatting.
    REQUIRE( PT(parseNumber,
        NumberToken(6)
        ) ==
        ~NumberExp(6)
        );

    REQUIRE( PT(parseVariable,
        VariableToken("fun")
        ) ==
        ~VariableExp("fun")
        );
    
    REQUIRE( PT(parseAdditive,
        NumberToken(1),
        PlusToken(),
        NumberToken(1)
        ) ==
        ~BinopExp(
            ~NumberExp(1),
            opPlus,
            ~NumberExp(1)
            )
        );

    REQUIRE( PT(parseUnary,
                MinusToken(),
                NumberToken(1)
                ) ==
             ~UnopExp(~NumberExp(1), opMinus)
            );

    REQUIRE( PT(parseMultive,
                NumberToken(2),
                MultiplyToken(),
                NumberToken(3)
                ) ==
             ~BinopExp(
                ~NumberExp(2),
                opMult,
                ~NumberExp(3)
                 )
             );


}
