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


    class NumberToken : public virtual Token
    {
    public:
        NumberToken();
        int num;
        TokType getType();
    };

    class VariableToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class FunctionToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class IfToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class ElseToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class PlusToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class MinusToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class MultiplyToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class DivisionToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class LeftBraceToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class RightBraceToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class LeftParenToken : public virtual Token
    {
    public:
        TokType getType();
    };

    class RightParenToken : public virtual Token
    {
    public:
        TokType getType();
    };

//    class LogicalAndToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class LogicalOrToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class BangToken : public virtual Token//!
//    {
//    public:
//        TokType getType();
//    };

//    class LeftBracketToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class RightBracketToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class EqualToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class PointToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class ForToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class WhileToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

//    class NewlineToken : public virtual Token
//    {
//    public:
//        TokType getType();
//    };

} //namespace dflat

#endif // TOKEN_HPP
