#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "string.hpp"
#include <memory>

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

		virtual operator String() const = 0;
    };

    using TokenPtr = std::unique_ptr<Token>;


    class NumberToken : public Token
    {
    public:
        NumberToken(int);
        TokType getType() const { return tokNum; }
        int num;
		
		operator String() const;
    };

    class VariableToken : public Token
    {
    public:
        VariableToken(String const&);
        TokType getType() const { return tokVar; }
        String name;

		operator String() const;
    };

    class IfToken : public Token
    {
    public:
        TokType getType() const { return tokIf; }

		operator String() const;
    };

    class ElseToken : public Token
    {
    public:
        TokType getType() const { return tokElse; }

		operator String() const;
    };

    class PlusToken : public Token
    {
    public:
        TokType getType() const { return tokPlus; }

		operator String() const;
    };

    class MinusToken : public Token
    {
    public:
        TokType getType() const { return tokMinus; }

		operator String() const;
    };

    class MultiplyToken : public Token
    {
    public:
        TokType getType() const { return tokMult; }

		operator String() const;
    };

    class DivisionToken : public Token
    {
    public:
        TokType getType() const { return tokDiv; }

		operator String() const;
    };

    class EqualToken : public Token
    {
    public:
        TokType getType() const { return tokEqual; }

		operator String() const;
    };

    class LeftBraceToken : public Token
    {
    public:
        TokType getType() const { return tokLBrace; }

		operator String() const;
    };

    class RightBraceToken : public Token
    {
    public:
        TokType getType() const { return tokRBrace; }

		operator String() const;
    };

    class LeftParenToken : public Token
    {
    public:
        TokType getType() const { return tokLParen; }

		operator String() const;
    };

    class RightParenToken : public Token
    {
    public:
        TokType getType() const { return tokRParen; }

		operator String() const;
    };

    class NewlineToken : public Token
    {
    public:
        TokType getType() const { return tokNewLine; }

		operator String() const;
    };

    class ForToken : public Token
    {
    public:
        TokType getType() const { return tokFor; }

		operator String() const;
    };

    class WhileToken : public Token
    {
    public:
        TokType getType() const { return tokWhile; }

		operator String() const;
    };

    class AndToken : public Token
    {
    public:
        TokType getType() const { return tokLogic; }

		operator String() const;
    };

	class OrToken : public Token
	{
	public:
		TokType getType() const { return tokLogic; }

		operator String() const;
	};

    class NotToken : public Token //!
    {
    public:
        TokType getType() const { return tokNot; }

		operator String() const;
    };

} //namespace dflat

#endif // TOKEN_HPP
