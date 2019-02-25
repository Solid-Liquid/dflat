#include "parser.hpp"

namespace dflat
{

using namespace std;

class Parser
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

    public:
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

        ASNPtr parseVariable();
        ASNPtr parseNumber();
        ASNPtr parseExp();
};

//////// DELETE THESE ON MERGE
class VarExp : public ASN
{
    public:
        String name;
        VarExp(String const& name_)
            : name(name_)
        {}
        String toString() { return "(Var " + name + ")"; }
};

class NumExp : public ASN
{
    public:
        int value;
        NumExp(int value_)
            : value(value_)
        {}
        String toString() { return "(Num " + to_string(value) + ")"; }
};
////////

#define MATCH(var, Tok) auto const* var = match<Tok>(); if (!var) { return nullptr; }

ASNPtr Parser::parseVariable()
{
    MATCH(var, VariableToken);
    return make_unique<VarExp>(var->name);
}

ASNPtr Parser::parseNumber()
{
    MATCH(num, NumberToken);
    return make_unique<NumExp>(num->num);
}

ASNPtr Parser::parseExp()
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

} //namespace dflat
