#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "string.hpp"
#include <memory>

namespace dflat
{
    enum TokType { tokNum, tokVar, tokIf, tokElse, tokPlus, tokMinus,
                    tokDiv, tokEqual, tokMult, tokRBrace, tokLBrace,
                    tokLParen, tokRParen, tokNewLine, tokFor, tokWhile,
                    tokAnd, tokNot, tokOr };

    class Token
    {
    public:
        virtual ~Token();
        virtual TokType getType() const = 0;
        virtual operator String() const = 0;
        virtual String toString() const = 0;

        template <typename T>
        T* as()
        {
            return dynamic_cast<T*>(this);
        }
        
        template <typename T>
        T const* as() const
        {
            return dynamic_cast<T const*>(this);
        }
    };

    using TokenPtr = std::unique_ptr<Token>;


    class NumberToken : public Token
    {
    public:
        int num;
        NumberToken(int);
        TokType getType() const { return tokNum; }
        operator String() const { return to_string(num); }
        String toString() const { return to_string(num); }
    };

    class VariableToken : public Token
    {
    public:
        String name;
        VariableToken(String const&);
        TokType getType() const { return tokVar; }
        operator String() const { return name; }
        String toString() const { return name; }
    };

    class IfToken : public Token
    {
    public:
        TokType getType() const { return tokIf; }
        operator String() const { return "if"; }
        String toString() const { return "if"; }
    };

    class ElseToken : public Token
    {
    public:
        TokType getType() const { return tokElse; }
        operator String() const { return "else"; }
        String toString() const { return "else"; }
    };

    class PlusToken : public Token
    {
    public:
        TokType getType() const { return tokPlus; }
        operator String() const { return "+"; }
        String toString() const { return "+"; }
    };

    class MinusToken : public Token
    {
    public:
        TokType getType() const { return tokMinus; }
        operator String() const { return "-"; }
        String toString() const { return "-"; }
    };

    class MultiplyToken : public Token
    {
    public:
        TokType getType() const { return tokMult; }
        operator String() const { return "*"; }
        String toString() const { return "*"; }
    };

    class DivisionToken : public Token
    {
    public:
        TokType getType() const { return tokDiv; }
        operator String() const { return "/"; }
        String toString() const { return "/"; }
    };

    class EqualToken : public Token
    {
    public:
        TokType getType() const { return tokEqual; }
        operator String() const { return "="; }
        String toString() const { return "="; }
    };

    class LeftBraceToken : public Token
    {
    public:
        TokType getType() const { return tokLBrace; }
        operator String() const { return "{"; }
        String toString() const { return "{"; }
    };

    class RightBraceToken : public Token
    {
    public:
        TokType getType() const { return tokRBrace; }
        operator String() const { return "}"; }
        String toString() const { return "}"; }
    };

    class LeftParenToken : public Token
    {
    public:
        TokType getType() const { return tokLParen; }
        operator String() const { return "("; }
        String toString() const { return "("; }
    };

    class RightParenToken : public Token
    {
    public:
        TokType getType() const { return tokRParen; }
        operator String() const { return ")"; }
        String toString() const { return ")"; }
    };

    class NewlineToken : public Token
    {
    public:
        TokType getType() const { return tokNewLine; }
        operator String() const { return "\n"; }
        String toString() const { return "\n"; }
    };

    class ForToken : public Token
    {
    public:
        TokType getType() const { return tokFor; }
        operator String() const { return "for"; }
        String toString() const { return "for"; }
    };

    class WhileToken : public Token
    {
    public:
        TokType getType() const { return tokWhile; }
        operator String() const { return "while"; }
        String toString() const { return "while"; }
    };

    class AndToken : public Token
    {
    public:
        TokType getType() const { return tokAnd; }
        operator String() const { return "&&"; }
        String toString() const { return "&&"; }
    };

	class OrToken : public Token
	{
	public:
        TokType getType() const { return tokOr; }
        operator String() const { return "||"; }
        String toString() const { return "||"; }
	};

    class NotToken : public Token //!
    {
    public:
        TokType getType() const { return tokNot; }
        operator String() const { return "!"; }
        String toString() const { return "!"; }
    };

} //namespace dflat

#endif // TOKEN_HPP
