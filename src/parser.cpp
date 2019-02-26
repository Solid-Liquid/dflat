#include "parser.hpp"

namespace dflat
{

using namespace std;

#define ENABLE_ROLLBACK auto rollbacker = Rollbacker(*this, _tokenPos)
#define CANCEL_ROLLBACK rollbacker.disable()

TokenPtr const& Parser::cur() const
{
    if (_tokenPos >= _tokens.size())
    {
        return _end;
    }

    return _tokens[_tokenPos];
}

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

OpType Parser::parseUnaryOp()
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

OpType Parser::parseMultiveOp()
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

OpType Parser::parseAdditiveOp()
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

OpType Parser::parseLogicalOp()
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

ASNPtr Parser::parseVariable()
{
    MATCH(var, VariableToken);
    return make_unique<VariableExp>(var.name);
}

ASNPtr Parser::parseNumber()
{
    MATCH(num, NumberToken);
    return make_unique<NumberExp>(num.num);
}

ASNPtr Parser::parseUnary()
{
    // op prim
    ENABLE_ROLLBACK;

    PARSE(op, parseUnaryOp());
    PARSE(prim, parsePrimary());

    CANCEL_ROLLBACK;
    return make_unique<UnopExp>(move(prim), op);
}

ASNPtr Parser::parseMethodCall()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseNew()
{
    // new + type + ( + exp + exp* + )
    return nullptr; //TODO
}

ASNPtr Parser::parseParensExp()
{
    ENABLE_ROLLBACK;
   
    MATCH_(LeftParenToken);
    PARSE(exp, parseExp());
    MATCH_(RightParenToken);
    
    CANCEL_ROLLBACK;
    return exp;
}

ASNPtr Parser::parsePrimary()
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

ASNPtr Parser::parseMultive()
{
    ENABLE_ROLLBACK;
    
    PARSE(left, parsePrimary());
    PARSE(op, parseMultiveOp());
    PARSE(right, parseMultive());
    
    CANCEL_ROLLBACK;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseAdditive()
{
    ENABLE_ROLLBACK;

    PARSE(left, parseMultive());
    PARSE(op, parseAdditiveOp());
    PARSE(right, parseAdditive());

    CANCEL_ROLLBACK;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseLogical()
{
    ENABLE_ROLLBACK;

    PARSE(left, parseAdditive());
    PARSE(op, parseLogicalOp());
    PARSE(right, parseLogical());
    
    CANCEL_ROLLBACK;
    return make_unique<BinopExp>(move(left), op, move(right));
}

ASNPtr Parser::parseExp()
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

ASNPtr Parser::parseVarDecl()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseAssignStmt()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseMemberAssignStmt()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseIfStmt()
{
    //  incomplete
    //  only one statement accepted in each brace
    /*
    if(exp)
    {
        exp
    }
    else
    {
        exp
    }
    */
   
    ENABLE_ROLLBACK;
    MATCH_(IfToken);
    MATCH_(LeftParenToken);
    PARSE(logicExp, parseExp());
    MATCH_(RightParenToken);
    MATCH_(LeftBraceToken);
    PARSE(trueStatements, parseBlock());
    MATCH_(RightBraceToken);
    MATCH_(ElseToken);
    MATCH_(LeftBraceToken);
    PARSE(falseStatements, parseBlock());
    MATCH_(RightBraceToken);
    
    CANCEL_ROLLBACK;
    return make_unique<IfBlock>(
         move(logicExp), 
         move(trueStatements), 
         move(falseStatements)
         );
}

ASNPtr Parser::parseWhileStmt()
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

ASNPtr Parser::parseForStmt()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseStmt()
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

ASNPtr Parser::parseBlock()
{
    return nullptr; //TODO
}

ASNPtr Parser::parseProgram()
{
    return nullptr; // TODO
}

Parser::Parser(Vector<TokenPtr> const& tokens)
    : _tokens(tokens)
    , _tokenPos(0)
    , _end(make_unique<EndToken>())
{}

} //namespace dflat
