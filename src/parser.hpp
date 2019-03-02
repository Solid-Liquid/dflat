#pragma once

#include "parser.hpp"
#include "token.hpp"
#include "vector.hpp"
#include "string.hpp"
#include "asn.hpp"
#include "tracer.hpp"

namespace dflat
{

class ParserException : public std::exception
{
public:
    using std::exception::exception;
};

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

    Vector<TokenPtr> const& _tokens;
    size_t _tokenPos;
    TokenPtr const _end;
    Tracer _tracer;

    TokenPtr const& cur() const;
    void next();

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
    
public:
    OpType parseUnaryOp();
    OpType parseMultiveOp();
    OpType parseAdditiveOp();
    OpType parseLogicalOp();
    ASNPtr parseVariable();
    ASNPtr parseNumber();
    ASNPtr parseUnary();
    ASNPtr parseMethodCall();
    ASNPtr parseNew();
    ASNPtr parseParensExp();
    ASNPtr parsePrimary();
    ASNPtr parseMultive();
    ASNPtr parseMultiveDown();
    ASNPtr parseAdditive();
    ASNPtr parseAdditiveDown();
    ASNPtr parseLogical();
    ASNPtr parseLogicalDown();
    ASNPtr parseExp();
    ASNPtr parseVarDecl();
    ASNPtr parseAssignStmt();
    ASNPtr parseMemberAssignStmt();
    ASNPtr parseIfStmt();
    ASNPtr parseWhileStmt();
    ASNPtr parseForStmt();
    ASNPtr parseStmt();
    ASNPtr parseBlock();
    ASNPtr parseProgram();
    Parser(Vector<TokenPtr> const&);
    ~Parser();
};

} // namespace dflat
