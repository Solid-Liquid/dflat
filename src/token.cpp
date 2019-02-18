#include "token.hpp"

namespace dflat
{

//Token:
Token::~Token()
{
}

//NumberToken:
NumberToken::NumberToken()
{
    num = 0;
}

TokType NumberToken::getType()
{
    return tokNum;
}

//VariableToken:
TokType VariableToken::getType()
{
    return tokVar;
}

//FunctionToken:
TokType FunctionToken::getType()
{
    return tokFunc;
}

//IfToken:
TokType IfToken::getType()
{
    return tokIf;
}

//ElseToken:
TokType ElseToken::getType()
{
    return tokElse;
}

//PlusToken:
TokType PlusToken::getType()
{
    return tokPlus;
}

//MinusToken:
TokType MinusToken::getType()
{
    return tokMinus;
}

//DivisionToken:
TokType DivisionToken::getType()
{
    return tokDiv;
}

//MultiplyToken:
TokType MultiplyToken::getType()
{
    return tokMult;
}

//LeftBraceToken:
TokType LeftBraceToken::getType()
{
    return tokLBrace;
}

//RightBraceToken:
TokType RightBraceToken::getType()
{
    return tokRBrace;
}

//LeftParenToken:
TokType LeftParenToken::getType()
{
    return tokLParen;
}

//RightBraceToken:
TokType RightParenToken::getType()
{
    return tokRParen;
}

} //namespace dflat
