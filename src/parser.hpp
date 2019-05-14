#pragma once

#include "parser.hpp"
#include "token.hpp"
#include "vector.hpp"
#include "optional.hpp"
#include "string.hpp"
#include "asn.hpp"
#include "tracer.hpp"

namespace dflat
{

Vector<ASNPtr> parse(Vector<TokenPtr> const&);  //Main runner function for parser

// TODO line/column in parse errors (and the rest if possible)
class ParserException : public std::runtime_error
{
public:
    ParserException(String msg) noexcept;
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
    String currentClass;
    bool hasMainMethod;

    Map<String, ClassDecl*> _classes;

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
    
public:
    Optional<String> parseName();
    Optional<OpType> parseUnaryOp();
    Optional<OpType> parseMultiveOp();
    Optional<OpType> parseAdditiveOp();
    Optional<OpType> parseLogicalOp();
    Optional<Variable> parseVariable();
    Optional<FormalArg> parseFormalArg();
    ASNPtr parseVariableExp();
    ASNPtr parseNumber();
    ASNPtr parseBoolTrue();
    ASNPtr parseBoolFalse();
    ASNPtr parseUnary();
    ASNPtr parseMemberExp();
    ASNPtr parseMethodExp();
    ASNPtr parseMethodStm();
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
    ASNPtr parseVarAssignDecl();
    ASNPtr parseVarDecl();
    ASNPtr parseAssignStm();
    ASNPtr parseIfStm();
    ASNPtr parsePrintStm();
    ASNPtr parseWhileStm();
    ASNPtr parseStm();
    BlockPtr parseBlock();
    ASNPtr parseMethodDecl();
    ASNPtr parseConsDecl();
    ASNPtr parseClassDecl();
    ASNPtr parseClassStm();
    Vector<ASNPtr> parseProgram();
    ASNPtr parseRetStm();
    Parser(Vector<TokenPtr> const&, bool requireMain = true);
    ~Parser();
};

} // namespace dflat
