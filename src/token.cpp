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

//VariableToken:
VariableToken::VariableToken(String const& name_)
    : name(name_)
{
}

} //namespace dflat
