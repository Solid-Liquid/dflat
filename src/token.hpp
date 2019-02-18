#ifndef TOKEN_HPP
#define TOKEN_HPP

namespace dflat
{

    class Token
    {
    public:
        Token();
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };


    class NumberToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class VariableToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class FunctionToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class IfToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class ThenToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();    };

    class ElseToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class PlusToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class MinusToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class MultiplyToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class DivisionToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class LogicalAndToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class LogicalOrToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class BangToken //!
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class LeftParenToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class RightParenToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class LeftBracketToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class RightBracketToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class LeftBraceToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class RightBraceToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class EqualToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class PointToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class ForToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class WhileToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class NewlineToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class NewToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

    class PrintlineToken
    {
    public:
        TokType hashCode();
        bool equals(const Token other);
        string toString();
    };

} //namespace dflat

#endif // TOKEN_HPP
