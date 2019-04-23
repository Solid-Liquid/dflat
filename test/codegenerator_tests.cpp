//Unit tests for Code Generation

#include "catch2/catch.hpp"
#include "typechecker.hpp"
#include "codegenerator.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace dflat;

GenEnv testGenEnv()
{
    ValueType objectType("Object");
    CanonName methodName("method", MethodType(voidType, {}));

    TypeEnv typeEnv;
    typeEnv.enterClass(objectType);
    typeEnv.addClassVar("member", intType);
    typeEnv.addClassMethod(methodName);
    typeEnv.leaveClass();

    GenEnv genEnv(typeEnv);
    genEnv.enterClass(objectType);
    genEnv.enterMethod(methodName);
    genEnv.declareLocal("var", intType);
    genEnv.declareLocal("obj", objectType);
    return genEnv;
}

// Strip tabs and newlines from string
// Note: This will break testing strings with \t or \n in them
String cleanOutput(String const& s)
{
    String t;
    t.reserve(s.size());

    for (char c : s)
    {
        switch (c)
        {
            case '\r':
            case '\n':
            case '\t':
                break;
            default:
                t += c;
        }
    }

    return t;
}

String codeGenExp(String const& input)
{
    //Helper function that makes testing expression code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseExp() and generateCode(). (No typchecking)
    GenEnv env = testGenEnv();
    auto result = Parser(tokenize(input)).parseExp();
    
    if (!result)
    {
        throw ParserException("Test failed to parse");
    }
    
    result->generateCode(env);
    return cleanOutput(env.concat());
}

String codeGenStm(String const& input)
{
    //Helper function that makes testing statement code generation less ugly
    //Takes a string, tokenizes it, passes it a parser instance,
    //calls parseStm() and generateCode(). (No typchecking)
    GenEnv env = testGenEnv();
    auto result = Parser(tokenize(input)).parseStm();
    
    if (!result)
    {
        throw ParserException("Test failed to parse");
    }
    
    result->generateCode(env);
    return cleanOutput(env.concat());
}

String codeGenProg(String const& input)
{
    Vector<ASNPtr> program = parse(tokenize(input));
    TypeEnv typeEnv = typeCheck(program);
    return cleanOutput(generateCode(program, typeEnv));
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
    REQUIRE( codeGenStm("int var = 1 + 2;") == "int df_var = (1+2);");

    REQUIRE( codeGenStm("int var = -2;") == "int df_var = (-2);");
    
    //Boolean declaration Statement:
    REQUIRE( codeGenStm("bool var = true;") == "int df_var = 1;"); //no bool in C code

    REQUIRE( codeGenStm("bool var = false;") == "int df_var = 0;"); //no bool in C code

    //Return statement:
    REQUIRE( codeGenStm("return 69;") == "return 69;");

    REQUIRE( codeGenStm("return var;") == "return df_var;");

    REQUIRE( codeGenStm("return 1 + 2 + 3;") == "return (1+(2+3));");

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

             "if ((!1)){int df_var = (1+2);}"

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

             "if ((1==0)){int df_var = (1+2);}else{int df_var = (1-2);}"

             );

    //While statement:
    REQUIRE( codeGenStm(R"(

                        while(true || false)
                        {
                            int var = 1+2;
                        }

                        )")

             ==

             "while ((1||0)){int df_var = (1+2);}"

             );
}

TEST_CASE( "Program-level Tests", "[CodeGenerator]" )
{
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int x;
            };
        )") == 
            "struct df_Base{int df_x;};"
        );
}
