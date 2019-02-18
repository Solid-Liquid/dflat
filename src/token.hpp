#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "string.hpp"

namespace dflat
{
    enum TokType { tokNum, tokVar, tokIf, tokElse, tokPlus, tokMinus, tokDiv,
                   tokEqual, tokMult, tokRBrace, tokLBrace, tokLParen, tokRParen,
                   tokNewLine, tokFor, tokWhile, tokLogic, tokNot };

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

    class EqualToken : public Token
    {
    public:
        TokType getType() const { return tokEqual; }
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

    class NewlineToken : public Token
    {
    public:
        TokType getType() const { return tokNewLine; }
    };

    class ForToken : public Token
    {
    public:
        TokType getType() const { return tokFor; }
    };

    class WhileToken : public Token
    {
    public:
        TokType getType() const { return tokWhile; }
    };

    class LogicalToken : public Token
    {
    public:
        TokType getType() const { return tokLogic; }
    };

    class NotToken : public Token //!
    {
    public:
        TokType getType() const { return tokNot; }
    };

} //namespace dflat

#endif // TOKEN_HPP
