//Unit tests for Code Generation

#include "catch2/catch.hpp"
#include "codegenerator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

TEST_CASE( "Expression Code Generation Tests", "[CodeGenerator]" )
{
    /*
     *   Tests for properly generating code for expressions (only):
     */

    //DOES CODE GENERATION THROW EXCEPTIONS?? MY GUESS IS NO,
    //ANY PROBLEMS WOULD BE HANDLED BEFOREHAND

    #define GENEXP(str) Parser(tokenize(str)).parseExp()->generateCode(env)
    GenEnv env;
    //Helper macro that makes testing expression code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseExp() and generateCode() for the expression.
    //"env" variable needed as arguement for generateCode()

    //REQUIRE( GENEXP("15") == "15");

    //REQUIRE( GENEXP("true") == "1");

    //REQUIRE( GENEXP("false") == "0");

    //REQUIRE( GENEXP("1 + 2") == "(1+2)");
}
