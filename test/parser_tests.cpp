//Unit tests for the parser

#include "catch2/catch.hpp"
#include "parser.hpp"
#include "token_helpers.hpp"

using namespace dflat;

// Convenience function for making ASN trees to test against.
template <typename T>
ASNPtr operator~(T&& t)
{
    return std::make_unique<T>(std::move(t));
}

#define PT(...) parse(tokens(__VA_ARGS__))

TEST_CASE( "Parser works correctly", "[parser]" )
{
    // Please forgive insane formatting.
    REQUIRE (PT(
        NumberToken(6)
        ) 
        ==
        ~NumberExp(6)
        );
}
