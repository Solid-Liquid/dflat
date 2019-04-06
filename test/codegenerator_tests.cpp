//Unit tests for Code Generation

#include "catch2/catch.hpp"
#include "codegenerator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

String codeGenExp(String const& input)
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

String codeGenStm(String const& input)
{
    //Helper function that makes testing statement code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseStm() and generateCode().
    String output;
    GenEnv env;
    Parser(tokenize(input)).parseStm()->generateCode(env);
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
    

    REQUIRE( codeGenExp("15") == "15");

    REQUIRE( codeGenExp("true") == "1");

    REQUIRE( codeGenExp("false") == "0");


    REQUIRE( codeGenExp("var") == "var");

    REQUIRE( codeGenExp("a.var") == "a->var");

    REQUIRE( codeGenExp("var + 2") == "(var+2)");

    REQUIRE( codeGenExp("a.var + 2") == "(a->var+2)");


    REQUIRE( codeGenExp("1 + 2") == "(1+2)");

    REQUIRE( codeGenExp("1 - 2") == "(1-2)");

    REQUIRE( codeGenExp("1 / 2") == "(1/2)");

    REQUIRE( codeGenExp("1 * 2") == "(1*2)");

    REQUIRE( codeGenExp("1 == 2") == "(1==2)");

    REQUIRE( codeGenExp("!2") == "(!2)");

    REQUIRE( codeGenExp("1 && 2") == "(1&&2)");

    REQUIRE( codeGenExp("1 + 2 + 3") == "(1+(2+3))");

    REQUIRE( codeGenExp("1 * 2 + 3 * 4 - 7") == "((1*2)+((3*4)-7))");

    REQUIRE( codeGenExp("true != false") == "(1!=0)");
}

TEST_CASE( "Statement Code Generation Tests", "[CodeGenerator]" )
{
    REQUIRE( codeGenStm("int var = 1 + 2;") == "int var = (1+2);\n");
    
    REQUIRE( codeGenStm("var = 1 + 2;") == "var = (1+2);\n");

    REQUIRE( codeGenStm("if(true == false){var = 1+2;}else{var = 1-2;}") == "if((1==0))\n{\nvar = (1+2);\n}\nelse\n{\nvar = (1-2);\n}\n");

    REQUIRE( codeGenStm("while(true || false){var = 1+2;}") == "while((1||0))\n{\nvar = (1+2);\n}\n");

    REQUIRE( codeGenStm("return 69;") == "return 69;\n");

    REQUIRE( codeGenStm("return var;") == "return var;\n");
    
    REQUIRE( codeGenStm("return 1 + 2 + 3;") == "return (1+(2+3));\n");
}