#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "string.hpp"

namespace dflat
{
    enum TokType { tokNum, tokVar, tokFunc, tokIf, tokElse, tokPlus, tokMinus, tokDiv, tokMult, tokRBrace, tokLBrace,
                 tokLParen, tokRParen };

    class Token
    {
    public:
        virtual ~Token();
        virtual TokType getType() const = 0;

        template <typename T>
        T* as()
        {
            return dynamic_cast<T*>(this);
        }
    };


    class NumberToken : public Token
    {
    public:
        NumberToken(int);
        TokType getType() const { return tokNum; }
        int num;
    };

    class VariableToken : public Token
    {
    public:
        VariableToken(String const&);
        TokType getType() const { return tokVar; }
        String name;
    };

    class FunctionToken : public Token
    {
    public:
        TokType getType() const { return tokFunc; }
    };

    class IfToken : public Token
    {
    public:
        TokType getType() const { return tokIf; }
    };

    class ElseToken : public Token
    {
    public:
        TokType getType() const { return tokElse; }
    };

    class PlusToken : public Token
    {
    public:
        TokType getType() const { return tokPlus; }
    };

    class MinusToken : public Token
    {
    public:
        TokType getType() const { return tokMinus; }
    };

    class MultiplyToken : public Token
    {
    public:
        TokType getType() const { return tokMult; }
    };

    class DivisionToken : public Token
    {
    public:
        TokType getType() const { return tokDiv; }
    };

    class LeftBraceToken : public Token
    {
    public:
        TokType getType() const { return tokLBrace; }
    };

    class RightBraceToken : public Token
    {
    public:
        TokType getType() const { return tokRBrace; }
    };

    class LeftParenToken : public Token
    {
    public:
        TokType getType() const { return tokLParen; }
    };

    class RightParenToken : public Token
    {
    public:
        TokType getType() const { return tokRParen; }
    };

//    class LogicalAndToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class LogicalOrToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class BangToken : public Token//!
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class LeftBracketToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class RightBracketToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class EqualToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class PointToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class ForToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class WhileToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

//    class NewlineToken : public Token
//    {
//    public:
//        TokType getType() const { return tok; }
//    };

} //namespace dflat

#endif // TOKEN_HPP
