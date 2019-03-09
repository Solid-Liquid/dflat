//Unit tests for the TypeChecker

#include "catch2/catch.hpp"
#include "typechecker.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

Type expType(String const& input)
{
    auto tokens = tokenize(input);
    Parser parser(tokens);
    auto asn = parser.parseExp();
    return typeCheck(asn);
}

TEST_CASE( "TypeChecker correctly checks types", "[TypeChecker]" )
{
    REQUIRE( expType("5")
            == intType );
    REQUIRE( expType("1 + 1")
            == intType );
    REQUIRE( expType("1 == 2")
            == boolType );
    
    /*
     *   Tests for Typechecker throwing exception:
     */

    REQUIRE_THROWS_AS(throw TypeCheckerException("error"), TypeCheckerException);
}
