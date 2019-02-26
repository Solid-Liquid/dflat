#include "parser.hpp"
#include "result.hpp"

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
    
    // Matches the current token but doesn't store it in a var.
    #define MATCH_(type) \
        if (!match<type>()) { return nullptr; } \
        /*end MATCH_*/

    Optional<OpType> parseMultiveOp()
    {
        switch (cur()->getType())
        {
            case tokMult:   return opMult;
            case tokDiv:    return opDiv;
            default:        return failure;
        }
    }
    
    Optional<OpType> parseAdditiveOp()
    {
        switch (cur()->getType())
        {
            case tokPlus:   return opPlus;
            case tokMinus:  return opMinus;
            default:        return failure;
        }
    }

    Optional<OpType> parseLogicalOp()
    {
        switch (cur()->getType())
        {
            case tokAnd:    return opAnd;
            case tokOr:     return opOr;
            case tokEq:     return opLogEq;
            case tokNotEq:  return opLogNotEq;
            default:        return failure;
        }
    }
    
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

    ASNPtr parseUnary()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseMethodCall()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseNew()
    {
        return nullptr; //TODO
    }

    ASNPtr parseParensExp()
    {
        //TODO rollback
        MATCH_(LeftParenToken);
        
        ASNPtr exp = parseExp();

        if (!exp)
        {
            return nullptr;
        }

        MATCH_(RightParenToken);
        return exp;
    }

    ASNPtr parsePrimary()
    {
        ASNPtr result;
        
        if (result = parseParensExp())
        {
            return result;
        }
        else if (result = parseUnary())
        {
            return result;
        }
        else if (result = parseMethodCall())
        {
            return result;
        }
        else if (result = parseNew())
        {
            return result;
        }
        else if (result = parseVariable())
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

    ASNPtr parseMultive()
    {
        //TODO rollback
        ASNPtr left = parsePrimary();

        if (!left)
        {
            return nullptr;
        }

        Optional<OpType> op = parseMultiveOp();

        if (!op)
        {
            return nullptr;
        }

        ASNPtr right = parseMultive();

        if (!right)
        {
            return nullptr;
        }

        return make_unique<BinopExp>(move(left), *op, move(right));
    }
    
    ASNPtr parseAdditive()
    {
        //TODO rollback
        ASNPtr left = parseMultive();

        if (!left)
        {
            return nullptr;
        }

        Optional<OpType> op = parseAdditiveOp();

        if (!op)
        {
            return nullptr;
        }

        ASNPtr right = parseAdditive();

        if (!right)
        {
            return nullptr;
        }

        return make_unique<BinopExp>(move(left), *op, move(right));
    }

    ASNPtr parseLogical()
    {
        //TODO rollback
        ASNPtr left = parseAdditive();

        if (!left) 
        {
            return nullptr;
        }

        Optional<OpType> op = parseLogicalOp();

        if (!op) 
        {
            return nullptr;
        }

        ASNPtr right = parseLogical();

        if (!right)
        {
            return nullptr;
        }
        
        return make_unique<BinopExp>(move(left), *op, move(right));
    }

    ASNPtr parseExp()
    {
        ASNPtr result;

        if (result = parseLogical())
        {
            return result;
        }
        else if (result = parsePrimary())
        {
            return result;
        }
        else
        {
            return nullptr;
        }
    }

public:
    Parser(Vector<TokenPtr> const& tokens)
        : _tokens(tokens)
        , _tokenPos(0)
        , _end(make_unique<EndToken>())
    {}

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

//Exp Parser::parseExp()
//{
//    const ParseResult<Exp> result = parseExp(0);
//    // TODO: Why does result->tokenPos not work?
//    if (result->tokenPos == tokens->size()) {
//        return result->result;
//    } else {
//        throw new ParserException("Extra tokens starting at " + result->tokenPos);
//    }
//}

//ParseResult::ParseResult(T&& result, std::size_t tokenPos)
//    : _result(std::move(result)), _tokenPos(tokenPos)
//{}

} //namespace dflat
