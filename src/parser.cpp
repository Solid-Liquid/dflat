#include "parser.hpp"

namespace dflat
{

using namespace std;

struct Parser
{
    Vector<TokenPtr> const& _tokens;
    size_t _tokenPos;
    TokenPtr const _end;

    TokenPtr const& cur() const
    {
        if (_tokenPos >= _tokens.size())
        {
            return _end;
        }

        return _tokens[_tokenPos];
    }

    void next()
    {
        if (_tokenPos >= _tokens.size())
        {
            return;
        }

        ++_tokenPos;
    }

    Parser(Vector<TokenPtr> const& tokens)
        : _tokens(tokens)
        , _tokenPos(0)
        , _end(make_unique<EndToken>())
    {}

    template <typename T>
    T const* match()
    {
        T const* t = cur()->as<T>();

        if (t)
        {
            next();
        }

        return t;
    }

// Matches the current token against a given token type.
//  On success, var is a reference to the current token,
//              and the current token advances.
//  On failure, the present function returns early with nullptr.
#define MATCH(var, type) \
    type const* var##__ = match<type>(); \
    if (!var##__) { return nullptr; } \
    type const& var = *var##__ \
    /*end MATCH*/

    ASNPtr parseVariable()
    {
        MATCH(var, VariableToken);
        return make_unique<VariableExp>(var.name);
    }
    
    ASNPtr parseNumber()
    {
        MATCH(num, NumberToken);
        return make_unique<NumberExp>(num.num);
    }
    
    ASNPtr parseExp()
    {
        ASNPtr result;

        if (result = parseVariable())
        {
            return result;
        }
        else if (result = parseNumber())
        {
            return result;
        }
        else
        {
            return nullptr;
        }
    }

    ASNPtr parseProgram()
    {
        return nullptr; // TODO
    }
};

ASNPtr parse(Vector<TokenPtr> const& tokens)
{
    Parser parser(tokens);
    return parser.parseProgram();
}


} //namespace dflat
