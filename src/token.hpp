#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

using namespace std;

namespace dflat
{
    enum TokType { tokNum, tokVar, tokFunc, tokIf, tokElse, tokPlus, tokMinus, tokDiv, tokMult, tokRBrace, tokLBrace,
                 tokLParen, tokRParen };

    class Token
    {
    public:
        virtual ~Token();
        virtual TokType getType() = 0;
    };


    class NumberToken : public Token
    {
    public:
        NumberToken();
        int num;
        TokType getType();
    };

    class VariableToken : public Token
    {
    public:
        TokType getType();
    };

    class FunctionToken : public Token
    {
    public:
        TokType getType();
    };

    class IfToken : public Token
    {
    public:
        TokType getType();
    };

    class ElseToken : public Token
    {
    public:
        TokType getType();
    };

    class PlusToken : public Token
    {
    public:
        TokType getType();
    };

    class MinusToken : public Token
    {
    public:
        TokType getType();
    };

    class MultiplyToken : public Token
    {
    public:
        TokType getType();
    };

    class DivisionToken : public Token
    {
    public:
        TokType getType();
    };

    class LeftBraceToken : public Token
    {
    public:
        TokType getType();
    };

    class RightBraceToken : public Token
    {
    public:
        TokType getType();
    };

    class LeftParenToken : public Token
    {
    public:
        TokType getType();
    };

    class RightParenToken : public Token
    {
    public:
        TokType getType();
    };

//    class LogicalAndToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class LogicalOrToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class BangToken : public Token//!
//    {
//    public:
//        TokType getType();
//    };

//    class LeftBracketToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class RightBracketToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class EqualToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class PointToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class ForToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class WhileToken : public Token
//    {
//    public:
//        TokType getType();
//    };

//    class NewlineToken : public Token
//    {
//    public:
//        TokType getType();
//    };

} //namespace dflat

#endif // TOKEN_HPP
