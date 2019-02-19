#include "token.hpp"

namespace dflat
{

//Token:
Token::~Token()
{
}



//NumberToken:
NumberToken::NumberToken(int num_)
    : num(num_)
{
}

NumberToken::operator String() const {
	return to_string(num);
}

//VariableToken:
VariableToken::VariableToken(String const& name_)
    : name(name_)
{
}

VariableToken::operator String() const {
	return name;
}

IfToken::operator String() const {
	return "if";
}

ElseToken::operator String() const
{
	return "else";
}

PlusToken::operator String() const
{
	return "+";
}

NotToken::operator String() const
{
	return "!";
}

AndToken::operator String() const
{
	return "&&";
}

OrToken::operator String() const
{
	return "||";
}

WhileToken::operator String() const
{
	return "while";
}

ForToken::operator String() const
{
	return "for";
}

NewlineToken::operator String() const
{
	return "\n";
}

RightParenToken::operator String() const
{
	return ")";
}

LeftParenToken::operator String() const
{
	return "(";
}

MinusToken::operator String() const
{
	return "-";
}

RightBraceToken::operator String() const
{
	return "}";
}

LeftBraceToken::operator String() const
{
	return "{";
}

EqualToken::operator String() const
{
	return "=";
}

DivisionToken::operator String() const
{
	return "/";
}

MultiplyToken::operator String() const
{
	return "*";
}

} //namespace dflat
