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
    TypeEnv env;
    env.enterClass(ValueType("Test"));
    return asn->typeCheck(env);
}


Type stmType(String const& input)
{
    //Helper to tokenize a statement
    //and calls "typeCheck" on it
    auto tokens = tokenize(input);
    Parser parser(tokens);
    auto asn = parser.parseStm();
    TypeEnv env;
    env.enterClass(ValueType("Test"));
    env.enterMethod(CanonName("test", MethodType(voidType, {})));
    return asn->typeCheck(env);
}

Vector<ASNPtr> parseTest(Vector<TokenPtr> const& tokens)
{
    //Calls parse with a flag that does not require a "Main" in dflat code
    Parser p(tokens,false);
    return p.parseProgram();
}
    
//TODO Base b = new Sub(); should typecheck.

TEST_CASE( "TypeChecker correctly checks types", "[TypeChecker]" )
{

    /*
     *  Tests for Typechecker returning correct type for expressions/statements:
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

    REQUIRE( stmType("int x = 5;")
             == voidType);

    REQUIRE( stmType("bool x = true;")
             == voidType);

    REQUIRE( stmType("Test x = new Test();")
             == voidType);

    REQUIRE( stmType("int x = 2 + -5 * 8;")
             == voidType);

    REQUIRE( stmType("int x = 5;")
             == voidType);

    REQUIRE( stmType("print(1);")
             == voidType);

    REQUIRE( stmType("print(true==false);")
             == voidType);
}



TEST_CASE( "TypeChecker checks structured code without exceptions","[TypeChecker]" )
{

    /*
     *   Tests for Typechecker successfully checking large blocks of code:
     */

    #define REQUIRE_TYPECHECKS(str) REQUIRE_NOTHROW(typeCheck(parseTest(tokenize(str))))
    //Helper macro that makes things less ugly for typecheck tests.
    //Calls tokenize, parse, and typecheck on a string.
    //Requires that no exceptions are thrown for test to succeed.


    //Class properly extends an existing class
    REQUIRE_TYPECHECKS(R"(

        class BaseClass {};
        class DerivedClass extends BaseClass{};

        )");

    //Instantiating polymorphic type
    REQUIRE_TYPECHECKS(R"(

        class BaseClass {};
        class DerivedClass extends BaseClass{};
        class Main
        {
            void main()
            {
                BaseClass x = new DerivedClass();
            }
        };
        )");

    //Demonstrates successful use and assignment of member variable "x":
    REQUIRE_TYPECHECKS(R"(

        class MyClass
        {
            int x;

            void f()
            {
                this.x = 5;
            }
        };

        )");

    //Method "f" is used as a standalone statement.
    REQUIRE_TYPECHECKS(R"(

        class MyClass
        {
            void f(int y)
            {
                y = 1 + 2;
            }

            void main()
            {
                this.f(2);
            }

        };

        )");
    
    //Correct type is returned for method "f" (int):
    //Method "f" is used in a expression that evaluates to int:
    REQUIRE_TYPECHECKS(R"(

        class MyClass
        {
            bool x;

            int f(int y)        // Comments
            {                   // included
                int x = 5;      // in this
                return x;       // test.
            }

            void main()
            {
                this.x = (f(5) == 1);
            }

        };

        )");
    
    //Demonstrates logical comparison. Also mixed Binop and Unop expression:
    REQUIRE_TYPECHECKS(R"(

        class MyClass
        {
            void main()
            {
                int x = 0;

                while (x != 10)
                {
                    x = x - -1;
                }
            }
        };

        )");
    
    //Successful typecheck with boolean return type and boolean expression:
    REQUIRE_TYPECHECKS(R"(

        class MyClass
        {
            bool f(int x)
            {
                if (x == 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };

        )");
    
    //Test of "class B" using members from "class A" in various ways:
    REQUIRE_TYPECHECKS(R"(

        class A
        {
            int x;
            int y;

            A A(int x, int y)
            {
                this.x = x;
                this.y = y;
            }
        };

        class B
        {
            A main()
            {
                A a1 = new A();
                A a2 = new A();
                a1.x = 1;
                a1.y = 1;
                a2.x = a1.x;
                a2.y = a1.y;
                return a2;
            }
        };

        )");
}




TEST_CASE( "TypeChecker properly throws exceptions", "[TypeChecker]" )
{
    /*
     *   Tests for Typechecker throwing exceptions:
     */

    #define REQUIRE_DOESNT_TYPECHECK(str) REQUIRE_THROWS_AS(typeCheck(parseTest(tokenize(str))),TypeCheckerException)
    //Helper macro that makes things less ugly for exception tests.
    //Calls tokenize, parse, and typecheck on a string.
    //Requires that a TypeCheckerException be thrown for the test to succeed.


    //Class redefenition error (two classes named "MyClass"):
    REQUIRE_DOESNT_TYPECHECK(R"(

        class MyClass {};
        class MyClass {};

        )");
    
    //Returning boolean for method that returns an int:
    REQUIRE_DOESNT_TYPECHECK(R"(

        class MyClass
        {
            int f()
            {
                return true;
            }
        };

        )");

    //Print only accepts boolean or int:
    REQUIRE_DOESNT_TYPECHECK(R"(

        class MyClass{};

        class Main
        {
            void main()
            {
                MyClass mc = new MyClass();
                print(mc);
            }
        };

        )");


    //Variable "var" is type bool. Expected RHS to be "bool" (instead it's int)
    REQUIRE_DOESNT_TYPECHECK(R"(

        class MyClass
        {
            bool method()
            {
                bool var = new int();
            }
        };

        )");

    //Class B has an object of type class A. Tries to use the object to
    //call a member variable "fakeMember" that does not exist.
    REQUIRE_DOESNT_TYPECHECK(R"(

        class A{};

        class B
        {
            A obj;

            void func()
            {
                obj.fakeMember = 5;
            }
        };

        )");

    //Class B has an object of type class A. Tries to use the object to
    //call a function that does not exist (obj.nonExistantMethod(); )
    REQUIRE_DOESNT_TYPECHECK(R"(

        class A{};

        class B
        {
            A obj;

            void func()
            {
                obj.nonExistantMethod();
            }
        };

        )");

    //Unkown type error ("junkType" is an invalid type):
    REQUIRE_THROWS_AS(TypeEnv().assertValidType(ValueType("junkType")),
                      TypeCheckerException);

    //Mismatched types. ("int" is not equivalent to "bool"):
    REQUIRE_THROWS_AS(TypeEnv().assertTypeIs(intType, boolType),
                      TypeCheckerException);

    //Invalid operands to operator:
    REQUIRE_THROWS_AS( expType("true + 5"),
                       TypeCheckerException);
}
