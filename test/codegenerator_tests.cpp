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
    //calls parseExp() and generateCode(). (No typchecking)
    String output;
    GenEnv env;
    Parser(tokenize(input)).parseExp()->generateCode(env);
    output += env.structDef.str()
            + env.funcDef.str()
            + env.main.str();
    return output;
}

String codeGenStm(String const& input)
{
    //Helper function that makes testing statement code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseStm() and generateCode(). (No typchecking)
    String output;
    GenEnv env;
    Parser(tokenize(input)).parseStm()->generateCode(env);
    output += env.structDef.str()
            + env.funcDef.str()
            + env.main.str();
    return output;
}

TEST_CASE( "Expression Code Generation Tests", "[CodeGenerator]" )
{
    /*
     *   Tests for properly generating code for expressions (only):
     */


    //Tests for literals:
    REQUIRE( codeGenExp("15") == "15");

    REQUIRE( codeGenExp("-4") == "(-4)");

    REQUIRE( codeGenExp("true") == "1"); //no bool in C code

    REQUIRE( codeGenExp("false") == "0"); //no bool in C code


    //Tests for variables:
    REQUIRE( codeGenExp("var") == "var"); //TODO: Append to variable/obj names

    REQUIRE( codeGenExp("obj.var") == "obj->var");


    //Tests for operator expressions:
    REQUIRE( codeGenExp("var + 2") == "(var+2)");

    REQUIRE( codeGenExp("a.var + 2") == "(a->var+2)");

    REQUIRE( codeGenExp("1 + 2") == "(1+2)");

    REQUIRE( codeGenExp("1 - 2") == "(1-2)");

    REQUIRE( codeGenExp("1 / 2") == "(1/2)");

    REQUIRE( codeGenExp("1 * 2") == "(1*2)");

    REQUIRE( codeGenExp("1 == 2") == "(1==2)");

    REQUIRE( codeGenExp("!2") == "(!2)");

    REQUIRE( codeGenExp("!true") == "(!1)");

    REQUIRE( codeGenExp("!(true || false)") == "(!(1||0))");

    REQUIRE( codeGenExp("1 && 2") == "(1&&2)");

    REQUIRE( codeGenExp("true || false") == "(1||0)");

    REQUIRE( codeGenExp("-(-3)") == "(-(-3))");

    REQUIRE( codeGenExp("1 + 2 + 3") == "(1+(2+3))");

    REQUIRE( codeGenExp("1 * 2 / 4") == "(1*(2/4))");

    REQUIRE( codeGenExp("1 * 2 + 3") == "((1*2)+3)");

    REQUIRE( codeGenExp("1 * 2 + 3 * 4 - 7") == "((1*2)+((3*4)-7))");

    REQUIRE( codeGenExp("true != false") == "(1!=0)"); //no bool in C code
}

TEST_CASE( "Statement Code Generation Tests", "[CodeGenerator]" )
{
    /*
     *   Tests for properly generating code for statements (only):
     *   Resulting code is one line to avoid white space that
     *   shouldn't be there.
     */

    //Integer Declaration Statement:
    REQUIRE( codeGenStm("int var = 1 + 2;") == "int var = (1+2);\n");

    REQUIRE( codeGenStm("int var = -2;") == "int var = (-2);\n");
    
    //Boolean declaration Statement:
    REQUIRE( codeGenStm("bool var = true;") == "int var = 1;\n"); //no bool in C code

    REQUIRE( codeGenStm("bool var = false;") == "int var = 0;\n"); //no bool in C code

    //Return statement:
    REQUIRE( codeGenStm("return 69;") == "return 69;\n");

    REQUIRE( codeGenStm("return var;") == "return var;\n");

    REQUIRE( codeGenStm("return 1 + 2 + 3;") == "return (1+(2+3));\n");

    ////If else statement:
    REQUIRE( codeGenStm(R"(

                        if(true == false)
                        {
                            var = 1+2;
                        }
                        else
                        {
                            var = 1-2;
                        }

                        )")

             ==

             "if((1==0))\n{\nvar = (1+2);\n}\nelse\n{\nvar = (1-2);\n}\n"

             );

    //While statement:
    REQUIRE( codeGenStm(R"(

                        while(true || false)
                        {
                            var = 1+2;
                        }

                        )")

             ==

             "while((1||0))\n{\nvar = (1+2);\n}\n"

             );
}
