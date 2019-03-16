//Unit tests for the TypeChecker

#include "catch2/catch.hpp"
#include "typechecker.hpp"
#include "lexer.hpp"
#include "token_helpers.hpp"

using namespace dflat;

Type expType(String const& input)
{
    //Helper function that tokenizes a string containing an expression
    //and calls "typeCheck" on it
    auto tokens = tokenize(input);
    Parser parser(tokens);
    auto asn = parser.parseExp();
    return typeCheck(asn);
}

Type stmType(String const& input)
{
    //Helper to tokenize a statement
    //and calls "typeCheck" on it
    auto tokens = tokenize(input);
    Parser parser(tokens);
    auto asn = parser.parseStm();
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

    REQUIRE( expType("2 - 1")
             == intType );

    REQUIRE( expType("2 * 2")
             == intType );

    REQUIRE( expType("2 / 2")
             == intType );

    REQUIRE( expType("1 + 1 - 2 * 4 / 2")
            == intType );

    REQUIRE( expType("1 == 2")
            == boolType );

    REQUIRE( expType("1 != 2")
             == boolType );


    REQUIRE( expType("1 && 0")
             == boolType );

    REQUIRE( expType("1 || 0")
             == boolType );

    // Assignment
//    REQUIRE( stmType("int x = 5;")
//             == voidType);
}

TEST_CASE( "TypeChecker checks structured code without exceptions",
        "[TypeChecker]" )
{
#define REQUIRE_TYPECHECKS(str) REQUIRE_NOTHROW(typeCheck(parse(tokenize(str))))
    REQUIRE_TYPECHECKS(R"(
        class MyClass
        {
            int x = 0;

            void f()
            {
                this.x = 5;
            }
        };
        )");
}


TEST_CASE( "TypeChecker properly throws exceptions", "[TypeChecker]" )
{
    /*
     *   Tests for Typechecker throwing exceptions:
     */

    //Class redefenition error (two classes named "MyClass"):
    REQUIRE_THROWS_AS(typeCheck(parse(tokenize(
                            "class MyClass {}; \
                             class MyClass {};"))),
                      TypeCheckerException);

    //Class extends error ("JunkClass" is not defined):
    REQUIRE_THROWS_AS(typeCheck(parse(tokenize(
                              "class MyClass extends JunkClass{}; "))),
                        TypeCheckerException);

    //Class cannot use instance of itself inside itself:
    REQUIRE_THROWS_AS(typeCheck(parse(tokenize(
                              "class MyClass                            \
                                {                                       \
                                    MyClass var = new MyClass();        \
                                };                                      "))),
                        TypeCheckerException);

    //Variable "var" is type bool. Expected RHS to be "bool" (instead it's int):
    REQUIRE_THROWS_AS(typeCheck(parse(tokenize(
                                "class MyClass                            \
                                  {                                       \
                                      bool var = new int();               \
                                  };                                      "))),
                          TypeCheckerException);

    //Unkown type error ("junkType" is an invalid type):
    REQUIRE_THROWS_AS(validType(initialTypeEnv(),"junkType"),
                      TypeCheckerException);

    //Mismatched types. ("int" is not equivalent to "bool"):
    REQUIRE_THROWS_AS(assertTypeIs(intType, boolType),
                      TypeCheckerException);
}
