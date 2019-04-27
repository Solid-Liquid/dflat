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
String strip(String const& s)
{
    String t;
    t.reserve(s.size());
    bool nl = true;

    for (char c : s)
    {
        switch (c)
        {
            case '\r':
                break;
            case '\n':
                nl = true;
                break;
            case '\t':
                break;
            case ' ':
                if (!nl)
                {
                    t += c;
                }
                break;
            default:
                nl = false;
                t += c;
                break;
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
    return strip(env.concat());
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
    return strip(env.concat());
}

String codeGenProg(String const& input)
{
    Vector<ASNPtr> program = parse(tokenize(input));
    TypeEnv typeEnv = typeCheck(program);
    return strip(generateCode(program, typeEnv));
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
    // Single class with single member var.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int x;
            };
        )") == strip(R"( 
            struct df_Base
            {
                int df_x;
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
        )"));
    
    // Single class with single method.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int f()
                {
                    return 1;
                }
            };
        )") == strip(R"(
            struct df_Base
            {
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
            int dfm_Base_f(struct df_Base* df_this)
            {
                return 1;
            }
        )"));
    
    // Simple inherited member var.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int x;
            };
            class Sub extends Base
            {
                int f()
                {
                    return x;
                }
            };
        )") == strip(R"(
            struct df_Base
            {
                int df_x;
            };
            struct df_Sub
            {
                struct df_Base dfparent;
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
           
            struct df_Sub* dfc_Sub(struct df_Sub* df_this)
            {
                return df_this;
            }
            int dfm_Sub_f(struct df_Sub* df_this)
            {
                return df_this->dfparent.df_x;
            }
        )"));
    
    // Double inherited member var.
    REQUIRE( codeGenProg(R"(
            class Base1
            {
                int x;
            };
            class Base2 extends Base1
            {
            };
            class Sub extends Base2
            {
                int f()
                {
                    return x;
                }
            };
        )") == strip(R"(
            struct df_Base1
            {
                int df_x;
            };
            struct df_Base2
            {
                struct df_Base1 dfparent;
            };
            struct df_Sub
            {
                struct df_Base2 dfparent;
            };
            
            struct df_Base1* dfc_Base1(struct df_Base1* df_this)
            {
                return df_this;
            }
           
            struct df_Base2* dfc_Base2(struct df_Base2* df_this)
            {
                return df_this;
            }
           
            struct df_Sub* dfc_Sub(struct df_Sub* df_this)
            {
                return df_this;
            }
            int dfm_Sub_f(struct df_Sub* df_this)
            {
                return df_this->dfparent.dfparent.df_x;
            }
        )"));

    // Inherited method.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int f()
                {
                    return 1;
                }
            };
            class Sub extends Base
            {
                int g()
                {
                    return f();
                }
            };
        )") == strip(R"(
            struct df_Base
            {
            };
            struct df_Sub
            {
                struct df_Base dfparent;
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
            int dfm_Base_f(struct df_Base* df_this)
            {
                return 1;
            }
           
            struct df_Sub* dfc_Sub(struct df_Sub* df_this)
            {
                return df_this;
            }
            int dfm_Sub_g(struct df_Sub* df_this)
            {
                return dfm_Base_f((struct df_Base*)df_this);
            }
        )"));
    
    // Basic instantiation.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                Base f()
                {
                    return new Base();
                }
            };
        )") == strip(R"(
            struct df_Base
            {
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
            struct df_Base* dfm_Base_f(struct df_Base* df_this)
            {
                return DF_NEW0(df_Base, dfc_Base);
            }
        )"));

    // Construction.
    // TODO Constructors.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                cons(int x, int y)
                {
                }
                Base f()
                {
                    return new Base(1,2);
                }
            };
        )") == strip(R"(
            struct df_Base
            {
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
            struct df_Base* dfc_Base_int_int(struct df_Base* df_this, int df_x, int df_y)
            {
                return df_this;
            }
            struct df_Base* dfm_Base_f(struct df_Base* df_this)
            {
                return DF_NEW(df_Base, dfc_Base_int_int, 1, 2);
            }
        )"));

    // Virtual construction and call.
    // TODO C virtual call.
    REQUIRE( codeGenProg(R"(
            class Base
            {
                int f()
                {
                    return 1;
                }
            };
            class Sub extends Base
            {
                int f()
                {
                    return 2;
                }
            };
            class Main
            {
                void main()
                {
                    Base base = new Base();
                    Base sub  = new Sub();
                    base.f();
                    sub.f();
                }
            };
        )") == strip(R"(
            struct df_Base
            {
            };
            struct df_Sub
            {
                struct df_Base dfparent;
            };
            struct df_Main
            {
            };
            
            struct df_Base* dfc_Base(struct df_Base* df_this)
            {
                return df_this;
            }
            int dfm_Base_f(struct df_Base* df_this)
            {
                return 1;
            }
            
            struct df_Sub* dfc_Sub(struct df_Sub* df_this)
            {
                return df_this;
            }
            int dfm_Sub_f(struct df_Sub* df_this)
            {
                return 2;
            }
            
            struct df_Main* dfc_Main(struct df_Main* df_this)
            {
                return df_this;
            }
            void dfm_Main_main(struct df_Main* df_this)
            {
                struct df_Base* df_base = DF_NEW0(df_Base, dfc_Base);
                struct df_Base* df_sub = DF_NEW0(df_Sub, dfc_Sub);
                dfm_Base_f(df_base);
                dfm_Base_f(df_sub);
            }
        )"));

}
