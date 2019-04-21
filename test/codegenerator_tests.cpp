//Unit tests for Code Generation

#include "catch2/catch.hpp"
#include "codegenerator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

void declTestStuff(GenEnv& env)
{
    // Tests use the following symbols. (See the other two functions below).
    env.classes.enter(ValueType("Object"));
    env.classes.addMember("member", ValueType("int"));
    env.classes.leave();
    env.scopes.declLocal("var", ValueType("int"));
    env.scopes.declLocal("obj", ValueType("Object"));
}

String codeGenExp(String const& input)
{
    //Helper function that makes testing expression code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseExp() and generateCode(). (No typchecking)
    GenEnv env;
    declTestStuff(env);
    Parser(tokenize(input)).parseExp()->generateCode(env);
    return env.concat();
}

String codeGenStm(String const& input)
{
    //Helper function that makes testing statement code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseStm() and generateCode(). (No typchecking)
    GenEnv env;
    declTestStuff(env);
    Parser(tokenize(input)).parseStm()->generateCode(env);
    return env.concat();
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
    REQUIRE( codeGenExp("var") == "df_var"); //TODO: Append to variable/obj names

    REQUIRE( codeGenExp("obj.member") == "df_obj->df_member");


    //Tests for operator expressions:
    REQUIRE( codeGenExp("var + 2") == "(df_var+2)");

    REQUIRE( codeGenExp("obj.member + 2") == "(df_obj->df_member+2)");

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
    REQUIRE( codeGenStm("int var = 1 + 2;") == "df_int df_var = (1+2);\n");

    REQUIRE( codeGenStm("int var = -2;") == "df_int df_var = (-2);\n");
    
    //Boolean declaration Statement:
    REQUIRE( codeGenStm("bool var = true;") == "df_int df_var = 1;\n"); //no bool in C code

    REQUIRE( codeGenStm("bool var = false;") == "df_int df_var = 0;\n"); //no bool in C code

    //Return statement:
    REQUIRE( codeGenStm("return 69;") == "return 69;\n");

    REQUIRE( codeGenStm("return var;") == "return df_var;\n");

    REQUIRE( codeGenStm("return 1 + 2 + 3;") == "return (1+(2+3));\n");

}

TEST_CASE( "Control Stuctures/Block Code Generation Tests", "[CodeGenerator]" )
{
    /*
     *   Tests for properly generating code for control structures/blocks:
     *   Resulting code is one line to avoid white space that
     *   shouldn't be there.
     */

    //If statement:
    REQUIRE( codeGenStm(R"(

                        if(!true)
                        {
                            int var = 1+2;
                        }

                        )")

             ==

             "if ((!1))\n{\ndf_int df_var = (1+2);\n}\n"

             );

    //If else statement:
    REQUIRE( codeGenStm(R"(

                        if(true == false)
                        {
                            int var = 1+2;
                        }
                        else
                        {
                            int var = 1-2;
                        }

                        )")

             ==

             "if ((1==0))\n{\ndf_int df_var = (1+2);\n}\nelse\n{\ndf_int df_var = (1-2);\n}\n"

             );

    //While statement:
    REQUIRE( codeGenStm(R"(

                        while(true || false)
                        {
                            int var = 1+2;
                        }

                        )")

             ==

             "while ((1||0))\n{\ndf_int df_var = (1+2);\n}\n"

             );
}
