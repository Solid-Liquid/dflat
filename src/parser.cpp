#include "parser.hpp"
#include "result.hpp"

namespace dflat
{

using namespace std;

class Parser
{
    class Rollbacker
    {
        Parser& _parser;
        size_t _oldPos;
        bool   _rollback;

    public:
        Rollbacker(Parser& parser, size_t oldPos)
            : _parser(parser)
            , _oldPos(oldPos)
            , _rollback(true)
        {}

        ~Rollbacker()
        {
            if (_rollback)
            {
                _parser._tokenPos = _oldPos;
            }
        }

        void disable()
        {
            _rollback = false;
        }
    };

    #define ENABLE_ROLLBACK auto rollbacker = Rollbacker(*this, _tokenPos)
    #define CANCEL_ROLLBACK rollbacker.disable()

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
   
    // Calls <parser> and stores the result in <var>.
    //  On success, var holds the result.
    //  On failure, the present function returns early with nullptr.
    #define PARSE(var, parser) \
        auto var = parser; \
        if (!var) { return nullptr; } \
        /*end PARSE*/

    // Combinator that calls a function and advances the current token
    //  if it was successful.
    template <typename T>
    auto advancing(T const& f)
    {
        auto r = f();
        
        if (r)
        {
            next();
        }
        
        return r;
    }
    
    OpType parseUnaryOp()
    {
        return advancing([this]()
        {
            switch (cur()->getType())
            {
                case tokNot:    return opNot;
                case tokMinus:  return opMinus;
                default:        return opNull;
            }
        });
    }

    OpType parseMultiveOp()
    {
        return advancing([this]()
        {
            switch (cur()->getType())
            {
                case tokMult:   return opMult;
                case tokDiv:    return opDiv;
                default:        return opNull;
            }
        });
    }

    OpType parseAdditiveOp()
    {
        return advancing([this]()
        {
            switch (cur()->getType())
            {
                case tokPlus:   return opPlus;
                case tokMinus:  return opMinus;
                default:        return opNull;
            }
        });
    }

    OpType parseLogicalOp()
    {
        return advancing([this]()
        {
            switch (cur()->getType())
            {
                case tokAnd:    return opAnd;
                case tokOr:     return opOr;
                case tokEq:     return opLogEq;
                case tokNotEq:  return opLogNotEq;
                default:        return opNull;
            }
        });
    }
   
    // EXPRESSION PARSERS

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
        ENABLE_ROLLBACK;

        PARSE(op, parseUnaryOp());
        PARSE(prim, parsePrimary());

        CANCEL_ROLLBACK;
        return make_unique<UnopExp>(op, move(prim));
    }
    
    ASNPtr parseMethodCall()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseNew()
    {
        // new + type + ( + exp + exp* + )
        return nullptr; //TODO
    }

    ASNPtr parseParensExp()
    {
        ENABLE_ROLLBACK;
       
        MATCH_(LeftParenToken);
        PARSE(exp, parseExp());
        MATCH_(RightParenToken);
        
        CANCEL_ROLLBACK;
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
        ENABLE_ROLLBACK;
        
        PARSE(left, parsePrimary());
        PARSE(op, parseMultiveOp());
        PARSE(right, parseMultive());
        
        CANCEL_ROLLBACK;
        return make_unique<BinopExp>(move(left), op, move(right));
    }
    
    ASNPtr parseAdditive()
    {
        ENABLE_ROLLBACK;

        PARSE(left, parseMultive());
        PARSE(op, parseAdditiveOp());
        PARSE(right, parseAdditive());

        CANCEL_ROLLBACK;
        return make_unique<BinopExp>(move(left), op, move(right));
    }

    ASNPtr parseLogical()
    {
        ENABLE_ROLLBACK;

        PARSE(left, parseAdditive());
        PARSE(op, parseLogicalOp());
        PARSE(right, parseLogical());
        
        CANCEL_ROLLBACK;
        return make_unique<BinopExp>(move(left), op, move(right));
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

    // STATEMENT PARSERS

    ASNPtr parseVarDecl()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseAssignStmt()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseMemberAssignStmt()
    {
        return nullptr; //TODO
    }
    
    ASNPtr parseIfStmt()
    {
        return nullptr; //TODO
    }
   
    ///// DELETE LATER //////
    class WhileBlock : public ASN
    {
    public:
        ASNPtr cond;
        ASNPtr body;

        WhileBlock(ASNPtr&& _cond, ASNPtr&& _body)
            : cond(move(_cond))
            , body(move(_body))
        {}

        String toString() const
        {
            return "while something";
        };

        ASNType getType() const
        {
            return {};
        }
    };
    ////////////////////

    ASNPtr parseWhileStmt()
    {
        ENABLE_ROLLBACK;
    
        MATCH_(WhileToken);
        MATCH_(LeftParenToken);
        PARSE(cond, parseExp());
        MATCH_(RightParenToken);
        PARSE(body, parseBlock());
        
        CANCEL_ROLLBACK;
        return make_unique<WhileBlock>(move(cond), move(body));
    }
    
    ASNPtr parseForStmt()
    {
        return nullptr; //TODO
    }

    ASNPtr parseStmt()
    {
        ASNPtr result;

        if (result = parseVarDecl())
        {
            return result;
        }
        else if (result = parseAssignStmt())
        {
            return result;
        }
        else if (result = parseMemberAssignStmt())
        {
            return result;
        }
        else if (result = parseIfStmt())
        {
            return result;
        }
        else if (result = parseWhileStmt())
        {
            return result;
        }
        else if (result = parseForStmt())
        {
            return result;
        }
        else if (result = parseExp())
        {
            return result;
        }
        else
        {
            return nullptr;
        }
    }

    // COMPOUND PARSERS
    
    ASNPtr parseBlock()
    {
        return nullptr; //TODO
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

// Exp Parser::parseExp()
// {
//     const ParseResult<Exp> result = parseExp(0);
//     // TODO: Why does result->tokenPos not work?
//     if (result->tokenPos == tokens->size()) {
//         return result->result;
//     } else {
//         throw new ParserException("Extra tokens starting at " + result->tokenPos);
//     }
// }

template<typename T>
ParseResult<T>::ParseResult(T&& result, std::size_t tokenPos)
    : _result(std::move(result)), _tokenPos(tokenPos)
{}

} //namespace dflat
