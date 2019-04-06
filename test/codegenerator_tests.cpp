//Unit tests for Code Generation

#include "catch2/catch.hpp"
#include "codegenerator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

String codeGen(String const& input)
{
    //Helper function that makes testing expression code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseExp() and generateCode().
    String output;
    GenEnv env;
    Parser(tokenize(input)).parseExp()->generateCode(env);
    output += env.structDef.str()
            + env.funcDef.str()
            + env.main.str();
//    std::cout << output << "\n";
    return output;
}

TEST_CASE( "Expression Code Generation Tests", "[CodeGenerator]" )
{
    /*
     *   Tests for properly generating code for expressions (only):
     */

    //DOES CODE GENERATION THROW EXCEPTIONS?? MY GUESS IS NO,
    //ANY PROBLEMS WOULD BE HANDLED BEFOREHAND
    

    REQUIRE( codeGen("15") == "15");

    REQUIRE( codeGen("true") == "1");

    REQUIRE( codeGen("false") == "0");

    REQUIRE( codeGen("1 + 2") == "(1+2)");

    REQUIRE( codeGen("1 - 2") == "(1-2)");

    REQUIRE( codeGen("1 / 2") == "(1/2)");

    REQUIRE( codeGen("1 * 2") == "(1*2)");

    REQUIRE( codeGen("1 == 2") == "(1==2)");

    REQUIRE( codeGen("!2") == "(!2)");

    REQUIRE( codeGen("1 && 2") == "(1&&2)");

    REQUIRE( codeGen("1 + 2 + 3") == "(1+(2+3))");
}
