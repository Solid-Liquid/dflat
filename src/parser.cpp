#include "parser.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

using namespace std;

#define TRACE _tracer.push(__func__ + String(" ") + cur()->toString() + " (" + to_string(_tokenPos) + ")")
#define SUCCESS _tracer.pop(TraceResult::success)
#define FAILURE _tracer.pop(TraceResult::failure)

#define ENABLE_ROLLBACK auto rollbacker = Rollbacker(*this, _tokenPos)
#define CANCEL_ROLLBACK rollbacker.disable()


Program parse(Vector<TokenPtr> const& tokens)
{
    Parser p(tokens);
    return p.parseProgram();
}

ParserException::ParserException(String msg) noexcept
{
    message = msg;
}

const char* ParserException::what() const noexcept
{
    return message.c_str();
}

/**
 * @brief Returns the current token or end of program token.
 * @return TokenPtr
 */
TokenPtr const& Parser::cur() const
{
    if (_tokenPos >= _tokens.size())
    {
        return _end;
    }

    return _tokens[_tokenPos];
}

/**
 * @brief Advances the reader if not at the end of the program.
 */
void Parser::next()
{
    if (_tokenPos >= _tokens.size())
    {
        return;
    }

    ++_tokenPos;
}

// Matches the current token against a given token type.
//  On success, var is a reference to the current token,
//              and the current token advances.
//  On failure, the present function returns early with nullptr.
#define MATCH(var, type) \
    type const* var##__ = match<type>(); \
    if (!var##__) { FAILURE; return nullptr; } \
    type const& var = *var##__ \
    /*end MATCH*/

// Matches the current token but doesn't store it in a var.
#define MATCH_(type) \
    if (!match<type>()) { FAILURE; return nullptr; } \
    /*end MATCH_*/

// Calls <parser> and stores the result in <var>.
//  On success, var holds the result.
//  On failure, the present function returns early with nullptr.
#define PARSE(var, parser) \
    auto var = parser; \
    if (!var) { FAILURE; return nullptr; } \
    /*end PARSE*/

OpType Parser::parseUnaryOp()
{
    TRACE;
    return advancing([this]()
    {
        switch (cur()->getType())
        {
            case tokNot:    SUCCESS; return opNot;
            case tokMinus:  SUCCESS; return opMinus;
            default:        FAILURE; return opNull;
        }
    });
}

OpType Parser::parseMultiveOp()
{
    TRACE;
    return advancing([this]()
    {
        switch (cur()->getType())
        {
            case tokMult:   SUCCESS; return opMult;
            case tokDiv:    SUCCESS; return opDiv;
            default:        FAILURE; return opNull;
        }
    });
}

OpType Parser::parseAdditiveOp()
{
    TRACE;
    return advancing([this]()
    {
        switch (cur()->getType())
        {
            case tokPlus:   SUCCESS; return opPlus;
            case tokMinus:  SUCCESS; return opMinus;
            default:        FAILURE; return opNull;
        }
    });
}

OpType Parser::parseLogicalOp()
{
    TRACE;
    return advancing([this]()
    {
        switch (cur()->getType())
        {
            case tokAnd:    SUCCESS; return opAnd;
            case tokOr:     SUCCESS; return opOr;
            case tokEq:     SUCCESS; return opLogEq;
            case tokNotEq:  SUCCESS; return opLogNotEq;
            default:        FAILURE; return opNull;
        }
    });
}

// EXPRESSION PARSERS

ASNPtr Parser::parseVariable()
{
    TRACE;
    MATCH(var, VariableToken);
    SUCCESS;
    return make_unique<VariableExp>(var.name);
}

ASNPtr Parser::parseNumber()
{
    TRACE;
    MATCH(num, NumberToken);
    SUCCESS;
    return make_unique<NumberExp>(num.num);
}

ASNPtr Parser::parseUnary()
{
    TRACE;
    // op prim
    ENABLE_ROLLBACK;

    PARSE(op, parseUnaryOp());
    PARSE(prim, parsePrimary());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<UnopExp>(move(prim), op);
}

ASNPtr Parser::parseMethodCall()
{
    // functionName + ( + exp + (, + exp)* + )
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> exps;
    ASNPtr temp;

    MATCH(var, VariableToken);
    MATCH_(LeftParenToken);
    temp = parseExp();
    if(temp)
    {
        exps.push_back(move(temp));
        while(match<CommaToken>())
        {
            PARSE(temp1, parseExp());
            exps.push_back(move(temp1));
        }
    }
    MATCH_(RightParenToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<MethodExp>(var.name, move(exps));
}

ASNPtr Parser::parseNew()
{
    // new + type + ( + exp + (, + exp)* + )
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> exps;
    ASNPtr temp;

    MATCH_(NewToken);
    MATCH(var, VariableToken);
    MATCH_(LeftParenToken);
    temp = parseExp();
    if(temp)
    {
        exps.push_back(move(temp));
        while(match<CommaToken>())
        {
            PARSE(temp1, parseExp());
            exps.push_back(move(temp1));
        }
    }
    MATCH_(RightParenToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<NewExp>(var.name, move(exps));
}

/**
 * @brief parseParensExp | Tries to parse everything inside of parenthesis.
 * @return ASNPtr
 */
ASNPtr Parser::parseParensExp()
{
    TRACE;
    ENABLE_ROLLBACK;

    MATCH_(LeftParenToken);
    PARSE(exp, parseExp());
    MATCH_(RightParenToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return exp;
}

ASNPtr Parser::parsePrimary()
{
    TRACE;
    ASNPtr result;

    if (result = parseNumber())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseParensExp())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseUnary())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseMethodCall())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseNew())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseVariable())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

ASNPtr Parser::parseMultive()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(left, parsePrimary());
    PARSE(op, parseMultiveOp());
    PARSE(right, parseMultiveDown());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseMultiveDown()
{
    TRACE;
    ASNPtr result;

    if (result = parseMultive())
    {
        SUCCESS;
        return result;
    }
    else if (result = parsePrimary())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

ASNPtr Parser::parseAdditive()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(left, parseMultiveDown());
    PARSE(op, parseAdditiveOp());
    PARSE(right, parseAdditiveDown());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseAdditiveDown()
{
    TRACE;
    ASNPtr result;

    if (result = parseAdditive())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseMultiveDown())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

ASNPtr Parser::parseLogical()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(left, parseAdditiveDown());
    PARSE(op, parseLogicalOp());
    PARSE(right, parseLogicalDown());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseLogicalDown()
{
    TRACE;
    ASNPtr result;

    if (result = parseLogical())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseAdditiveDown())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

ASNPtr Parser::parseExp()
{
    TRACE;
    ASNPtr result;

    if (result = parseLogicalDown())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

// STATEMENT PARSERS

ASNPtr Parser::parseVarDecl()
{
    TRACE;
    ENABLE_ROLLBACK;

    MATCH(varType, VariableToken);
    MATCH(varName, VariableToken);
    MATCH_(AssignToken);
    PARSE(exp, parseExp());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<VarDecStm>(varType.name, varName.name, move(exp));
}

ASNPtr Parser::parseAssignStm()
{
    TRACE;
    ENABLE_ROLLBACK;

    MATCH(varName, VariableToken);
    MATCH_(AssignToken);
    PARSE(exp, parseExp());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<AssignmentStm>(varName.name, move(exp));
}

ASNPtr Parser::parseMemberAssignStm()
{
    TRACE;
    FAILURE;
    return nullptr; //TODO
}

ASNPtr Parser::parseIfStm()
{
    TRACE;

    ENABLE_ROLLBACK;
    MATCH_(IfToken);
    MATCH_(LeftParenToken);
    PARSE(logicExp, parseExp());
    MATCH_(RightParenToken);
    PARSE(trueStatements, parseBlock());
    ASNPtr elseBlock;
    if( match<ElseToken>() )
    {
        PARSE(falseStatements, parseBlock());
        elseBlock = move(falseStatements);
    }
    else
    {
        elseBlock = make_unique<Block>(Vector<ASNPtr>{});
    }

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<IfStm>(
        move(logicExp),
        move(trueStatements),
        move(elseBlock)
        );
}

ASNPtr Parser::parseWhileStm()
{
    TRACE;
    ENABLE_ROLLBACK;

    MATCH_(WhileToken);
    MATCH_(LeftParenToken);
    PARSE(cond, parseExp());
    MATCH_(RightParenToken);
    PARSE(body, parseBlock());

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<WhileStm>(move(cond), move(body));
}

ASNPtr Parser::parseStm()
{
    TRACE;
    ASNPtr result;

    if (result = parseVarDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseAssignStm())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseMemberAssignStm())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseIfStm())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseWhileStm())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseExp())
    {
        SUCCESS;
        return result;
    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

// COMPOUND PARSERS

ASNPtr Parser::parseBlock()
{
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> stm;
    ASNPtr curstm = nullptr;

    MATCH_(LeftBraceToken);

    while((curstm = parseStm()))
    {
        stm.push_back(move(curstm));
    }

    MATCH_(RightBraceToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<Block>(move(stm));
}

Program Parser::parseProgram()
{
    TRACE;
    Program prog;
    bool run = true;
    ASNPtr result;

    while(run)
    {
        //TODO in the future, only calls parseClass()
        if(result = parseExp())
        {
            prog.classes.push_back(move(result));
        }
        else
        {
            run = false;
        }
    }

    if(!match<EndToken>())
    {
        FAILURE;
        String msg = "Unable to parse at position: " + to_string(_tokenPos);
        throw ParserException(msg);
    }

    return prog; // TODO
}

Parser::Parser(Vector<TokenPtr> const& tokens)
    : _tokens(tokens)
    , _tokenPos(0)
    , _end(make_unique<EndToken>())
    , _tracer("Parser(" + to_string(tokens) + ")", config::traceIndent)
{
}

Parser::~Parser()
{
    if (config::trace)
    {
        std::cout << "\n";
        _tracer.finalize();
        _tracer.print();
    }
}

} //namespace dflat
