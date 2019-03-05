//Unit tests for the parser

#include "catch2/catch.hpp"
#include "parser.hpp"
#include "token_helpers.hpp"
#include <iostream>

using namespace dflat;

// Convenience function for making ASN trees to test against.
template <typename T>
ASNPtr operator~(T&& t)
{
    return std::make_unique<T>(std::forward<T>(t));
}

template <typename... Ts>
Vector<ASNPtr> asns(Ts&&... in)
{
    Vector<ASNPtr> out;
    (out.push_back(std::make_unique<Ts>(in)), ...);
    return out;
}

//Parser( tokens(NumberToken(1), PlusToken(), NumberToken(1)) ).parseAdditive()
#define PT(method, ...) Parser(tokens(__VA_ARGS__)).method()

TEST_CASE( "Parser works correctly", "[parser]" )
{
    // Please forgive insane formatting :)
    // == denotes the comparison between input and expected output
    // PT[test function, token input...] == ~[resulting exp, block, or stm]


    /*
     * Tests for success on individual functions:
     */

    REQUIRE( PT(parseNumber, //6 -> NumberExp
        NumberToken(6)
        )
        ==
        ~NumberExp(6)
        );

    REQUIRE( PT(parseExp, //same as above but using parseExp
        NumberToken(6)
        )
        ==
        ~NumberExp(6)
        );

    REQUIRE( PT(parseVariable, //fun -> VariableExp
        VariableToken("fun")
        )
        ==
        ~VariableExp("fun")
        );

    REQUIRE( PT(parseExp,      //same as above but using parseExp
        VariableToken("fun")
        )
        ==
        ~VariableExp("fun")
        );
    
    REQUIRE( PT(parseAdditive,  //1 + 1 -> BinopExp(additive)
        NumberToken(1),
        PlusToken(),
        NumberToken(1)
        )
        ==
        ~BinopExp(
            ~NumberExp(1),
            opPlus,
            ~NumberExp(1)
            )
        );

    REQUIRE( PT(parseAdditive, //2 - 5 -> BinopExp(additive)
        NumberToken(2),
        MinusToken(),
        NumberToken(5)
        )
        ==
        ~BinopExp(
            ~NumberExp(2),
            opMinus,
            ~NumberExp(5)
            )
        );

    REQUIRE( PT(parseExp, //same as above but using parseExp
        NumberToken(2),
        MinusToken(),
        NumberToken(5)
        )
        ==
        ~BinopExp(
            ~NumberExp(2),
            opMinus,
            ~NumberExp(5)
            )
        );

    REQUIRE( PT(parseExp, // (2 - 5)  -> BinopExp(additive) from parentheses
        LeftParenToken(),
        NumberToken(2),
        MinusToken(),
        NumberToken(5),
        RightParenToken()
        )
        ==
        ~BinopExp(
            ~NumberExp(2),
            opMinus,
            ~NumberExp(5)
            )
        );

    REQUIRE( PT(parseMultive,     //2 * 3 -> BinopExp(multive)
                NumberToken(2),
                MultiplyToken(),
                NumberToken(3)
                )
             ==
             ~BinopExp(
                ~NumberExp(2),
                opMult,
                ~NumberExp(3)
                 )
             );

    REQUIRE( PT(parseMultive,     //10 / 5 -> BinopExp(multive)
                NumberToken(10),
                DivisionToken(),
                NumberToken(5)
                )
             ==
             ~BinopExp(
                ~NumberExp(10),
                opDiv,
                ~NumberExp(5)
                 )
             );

    REQUIRE( PT(parseExp,         //same as above but using parseExp
                NumberToken(10),
                DivisionToken(),
                NumberToken(5)
                )
             ==
             ~BinopExp(
                ~NumberExp(10),
                opDiv,
                ~NumberExp(5)
                 )
             );

    REQUIRE( PT(parseLogical,         //foo && bar -> BinopExp(logical)
                VariableToken("foo"),
                AndToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opAnd,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseLogical,         //foo || bar -> BinopExp(logical)
                VariableToken("foo"),
                OrToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opOr,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseLogical,         //same as above but using parseExp
                VariableToken("foo"),
                OrToken(),
                VariableToken("bar")
                )
             ==
             ~BinopExp(
                ~VariableExp("foo"),
                opOr,
                ~VariableExp("bar")
                 )
             );

    REQUIRE( PT(parseUnary,    //-1 -> UnaryExp
                MinusToken(),
                NumberToken(1)
                )
             ==
             ~UnopExp(~NumberExp(1), opMinus)
            );

    REQUIRE( PT(parseUnary,    //!var -> UnaryExp
                NotToken(),
                VariableToken("var")
                )
             ==
             ~UnopExp(~VariableExp("var"), opNot)
            );

    REQUIRE( PT(parseExp,    //same as above but using parseExp
                NotToken(),
                VariableToken("var")
                )
             ==
             ~UnopExp(~VariableExp("var"), opNot)
            );    

    REQUIRE( PT(parseExp,  //BinopExp(additive) with nested BinopExp(multive)
        NumberToken(1),    // 1 + 1 * 4
        PlusToken(),
        NumberToken(1),
        MultiplyToken(),
        NumberToken(4)
        )
        ==
        ~BinopExp(
            ~NumberExp(1),
            opPlus,
            ~BinopExp(
                ~NumberExp(1),
                opMult,
                ~NumberExp(4)
                )
            )
        );

    REQUIRE( PT(parseExp,  //BinopExp(additive) with nested BinopExp(multive)
        NumberToken(1),    // 1 * 1 + 4    ->   (1 * 1) + 4
        MultiplyToken(),
        NumberToken(1),
        PlusToken(),
        NumberToken(4)
        )
        ==
        ~BinopExp(
            ~BinopExp(~NumberExp(1),opMult,~NumberExp(1)),
            opPlus,
            ~NumberExp(4)
            )
        );

    REQUIRE( PT(parseExp,  //BinopExp(additive) with nested UnaryExp
        MinusToken(),      // -1 * 1    ->   (-1) * 1
        NumberToken(1),
        MultiplyToken(),
        NumberToken(1)
        )
        ==
        ~BinopExp(
            ~UnopExp(~NumberExp(1),opMinus),
            opMult,
            ~NumberExp(1)
            )
        );

    REQUIRE( PT(parseBlock,        // { }   ->   empty block
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~Block()
             );

    REQUIRE( PT(parseIfStm,         // If (1) { }  ->  IfStm
                IfToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~IfStm(~NumberExp(1),
                      ~Block(),
                      ~Block())
             );

    REQUIRE( PT(parseIfStm,         // if(1){} else{}  ->  If else stm
                IfToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken(),
                ElseToken(),
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~IfStm(~NumberExp(1),
                    ~Block(),
                    ~Block())
             );

    REQUIRE( PT(parseWhileStm,      //while(1){}  ->  while statement
                WhileToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~WhileStm(~NumberExp(1),
                       ~Block()
                       )
             );

    REQUIRE( PT(parseAssignStm,         //name = 1;  ->  Assignment statement
                VariableToken("name"),
                AssignToken(),
                NumberToken(1),
                SemiToken()
                )
             ==
             ~AssignmentStm("name", ~NumberExp(1))
             );

    REQUIRE( PT(parseVarDecl,           //type name = 1;  -> variable declaration
                VariableToken("type"),
                VariableToken("name"),
                AssignToken(),
                NumberToken(1),
                SemiToken()
                )
             ==
             ~VarDecStm("type", "name", ~NumberExp(1))
             );


    REQUIRE( PT(parseNew,       //new int()  ->  NewExp
                NewToken(),
                VariableToken("int"),
                LeftParenToken(),
                RightParenToken()
                )
             ==
             ~NewExp("int",
                      Vector<ASNPtr>{})
             );


    REQUIRE( PT(parseNew,       //new int(3) -> NewExp
                NewToken(),
                VariableToken("int"),
                LeftParenToken(),
                NumberToken(3),
                RightParenToken()
                )
             ==
             ~NewExp("int",
                      asns(NumberExp(3)))
             );


    REQUIRE( PT(parseNew,       //new int(3,suh)  -> NewExp
                NewToken(),
                VariableToken("int"),
                LeftParenToken(),
                NumberToken(3),
                CommaToken(),
                VariableToken("suh"),
                RightParenToken()
                )
             ==
             ~NewExp("int",
                      asns(NumberExp(3), VariableExp("suh")))
             );


    REQUIRE( PT(parseMethodExp,          //function()  ->  MethodExp
                VariableToken("function"), //TODO method call as exp vs as statement??
                LeftParenToken(),
                RightParenToken()
                )
             ==
             ~MethodExp("function",
                      Vector<ASNPtr>{})
             );


    REQUIRE( PT(parseMethodExp,            //function(3)  ->  MethodExp
                VariableToken("function"),
                LeftParenToken(),
                NumberToken(3),
                RightParenToken()
                )
             ==
             ~MethodExp("function",
                      asns(NumberExp(3)))
             );


    REQUIRE( PT(parseMethodExp,            //function(3,suh)  -> MethodExp
                VariableToken("function"),
                LeftParenToken(),
                NumberToken(3),
                CommaToken(),
                VariableToken("suh"),
                RightParenToken()
                )
             ==
             ~MethodExp("function",
                      asns(NumberExp(3), VariableExp("suh")))
             );

    REQUIRE( PT(parseMethodStm,            //function(3,suh);  -> MethodStm
                VariableToken("function"),
                LeftParenToken(),
                NumberToken(3),
                CommaToken(),
                VariableToken("suh"),
                RightParenToken(),
                SemiToken()
                )
             ==
             ~MethodStm(
                 ~MethodExp("function",asns(NumberExp(3), VariableExp("suh"))))
             );

    REQUIRE( PT(parseRetStm,        //return 1;  ->  ReturnStm
                ReturnToken(),
                NumberToken(1),
                SemiToken()
                )
             ==
            ~RetStm(~NumberExp(1))
             );

    REQUIRE( PT(parseClassDecl, // class MyClass { };  -> ClassDeclaration
                ClassToken(),
                VariableToken("MyClass"),
                LeftBraceToken(),
                RightBraceToken(),
                SemiToken()
                )
             ==
             ~ClassDecl("MyClass",Vector<ASNPtr>())
            );

    REQUIRE( PT(parseMethodDecl,            //int func(){ }  -> MethodDef
                VariableToken("int"),
                VariableToken("func"),
                LeftParenToken(),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken()
                )
             ==
             ~MethodDef("int","func",Vector<ASNPtr>(), ~Block())
             );

    /*
     * nullptr is properly returned for unsuccessful parse:
     */

    REQUIRE( PT(parseNumber,  //parse is not Number
        VariableToken("var")
        )
        ==
        nullptr
        );

    REQUIRE( PT(parseVariable, //parse is not Variable
        NumberToken(6)
        )
        ==
        nullptr
        );

    REQUIRE( PT(parseAdditive,      //parse is not Additive
                NumberToken(2)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseAdditive,      //parse is not Additive
                NumberToken(2),
                MultiplyToken(),
                NumberToken(3)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseMultive,      //parse is not Multive
                NumberToken(2),
                PlusToken(),
                NumberToken(3)
                )
             ==
             nullptr
             );

    REQUIRE( PT(parseUnary,           //parse is not Unary
                VariableToken("var")
                )
             ==
             nullptr
            );


    /*
     * exception is thrown for bad parse:
     */


    REQUIRE_THROWS_AS( PT(parseExp, // (2 - 5  -> missing )
        LeftParenToken(),
        NumberToken(2),
        MinusToken(),
        NumberToken(5)
        ),
        ParserException
        );

    REQUIRE_THROWS_AS( PT(parseMethodExp,          //function()  ->  missing )
                VariableToken("function"),
                LeftParenToken()
                ),
             ParserException
             );

    REQUIRE_THROWS_AS( PT(parseMethodExp,     //function(3,)  -> expected stm after ,
                VariableToken("function"),
                LeftParenToken(),
                NumberToken(3),
                CommaToken(),
                RightParenToken()
                ),
            ParserException
             );

    REQUIRE_THROWS_AS( PT(parseNew,       //new ()  ->  missing type
                NewToken(),
                LeftParenToken(),
                RightParenToken()
                ),
             ParserException
             );

    REQUIRE_THROWS_AS( PT(parseVarDecl, //type name = ;  -> expected expression
                VariableToken("type"),  //TODO semicolon?; declaration without assignment???
                VariableToken("name"),
                AssignToken()
                ),
             ParserException
             );

    REQUIRE_THROWS_AS( PT(parseVarDecl, //type name = 1 +;  -> expected expression
                VariableToken("type"),  //TODO semicolon?; declaration without assignment???
                VariableToken("name"),
                AssignToken(),
                NumberToken(1),
                PlusToken()
                ),
             ParserException
             );

    REQUIRE_THROWS_AS( PT(parseAdditive,  //1 + -> expected expression after '+'
        NumberToken(1),
        PlusToken()
        ),
        ParserException
        );

    REQUIRE_THROWS_AS( PT(parseUnary,  //- -> expected expression after unary -
                MinusToken()
                ),
            ParserException
            );

    REQUIRE_THROWS_AS( PT(parseAdditive,  //1 + - -> expected expresion after unary -
        NumberToken(1),
        PlusToken(),
        MinusToken()
        ),
        ParserException
        );

    REQUIRE_THROWS_AS( PT(parseIfStm,      // if(1){} else{}  ->  expected block after if
                IfToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken()
                ),
            ParserException
             );

    REQUIRE_THROWS_AS( PT(parseIfStm,   // if(1){} else{}  -> expected block after else
                IfToken(),
                LeftParenToken(),
                NumberToken(1),
                RightParenToken(),
                LeftBraceToken(),
                RightBraceToken(),
                ElseToken()
                ),
            ParserException
             );

    REQUIRE_THROWS_AS( PT(parseBlock,   // {  -> missing right bracket for block
                LeftBraceToken()
                ),
            ParserException
            );

    REQUIRE_THROWS_AS( PT(parseClassDecl, // class -> expected class name
                ClassToken()
                ),
            ParserException
            );
}
