#include "token.hpp"

namespace dflat
{

String to_string(TokenPtr const& token)
{
    return token->toString();
}

String to_string(Vector<TokenPtr> const& tokens)
{
    String s;
    bool first = true;
    
    for (TokenPtr const& t : tokens)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s += " ";
        }

        s += t->toString();
    }

    return s;
}

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
