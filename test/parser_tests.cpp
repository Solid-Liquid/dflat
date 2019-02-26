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

#define PT(method, ...) Parser(tokens(__VA_ARGS__)).method()

TEST_CASE( "Parser works correctly", "[parser]" )
{
    // Please forgive insane formatting.
//    REQUIRE( PT(parseNumber,
//        NumberToken(6)
//        ) ==
//        ~NumberExp(6)
//        );
//    
//    REQUIRE( PT(parseVariable,
//        VariableToken("fun")
//        ) ==
//        ~VariableExp("fun")
//        );
    
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
}
