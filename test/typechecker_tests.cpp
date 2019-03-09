//Unit tests for the TypeChecker

#include "catch2/catch.hpp"
#include "typechecker.hpp"
#include "lexer.hpp"
#include "token_helpers.hpp"

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

    /*
     *   Tests for Typechecker returning correct type for expressions:
     */

    REQUIRE( expType("5")
            == intType );

    REQUIRE( expType("1 + 1")
            == intType );

    REQUIRE( expType("1 == 2")
            == boolType );
}


TEST_CASE( "TypeChecker properly throws exceptions", "[TypeChecker]" )
{
    /*
     *   Tests for Typechecker throwing exceptions:
     */

    Vector<ASNPtr> program; //Can be used by tests. Clear after using.

    //Class redefenition error (two classes named "MyClass"):
    program.push_back(~ClassDecl("MyClass",Vector<ASNPtr>()));
    program.push_back(~ClassDecl("MyClass",Vector<ASNPtr>()));
    REQUIRE_THROWS_AS(typeCheck(program),TypeCheckerException);
    program.clear();

    //REQUIRE_THROWS_AS(,TypeCheckerException);
}
