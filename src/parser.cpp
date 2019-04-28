#include "parser.hpp"
#include "config.hpp"
#include <iostream>

namespace dflat
{

using namespace std;

#define TRACE _tracer.push(__func__ + String(" ") + cur()->toString() + " (" + to_string(_tokenPos) + ")")
#define SUCCESS _tracer.pop(traceSuccess)
#define FAILURE _tracer.pop(traceFailure)

#define ENABLE_ROLLBACK auto rollbacker = Rollbacker(*this, _tokenPos)
#define CANCEL_ROLLBACK rollbacker.disable()


Vector<ASNPtr> parse(Vector<TokenPtr> const& tokens)
{
    Parser p(tokens);
    return p.parseProgram();
}

ParserException::ParserException(String msg) noexcept
    : std::runtime_error("Parser Exception\n" + std::move(msg))
{
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
//  On nullopt, the present function returns early with nullptr:
#define MATCH(var, type) \
    type const* var##__ = match<type>(); \
    if (!var##__) { FAILURE; return {}; } \
    type const& var = *var##__ \
    /*end MATCH*/

//Same as MATCH, but instead of returning a nullptr, throws an exception
//expected is a string saying what should be matched:
#define MUST_MATCH(var, type, expected) \
    type const* var##__ = match<type>(); \
    if (!var##__) { FAILURE; \
        throw ParserException("Expected " + String(expected) \
            + " at position: " + to_string(_tokenPos)); } \
    type const& var = *var##__ \
    /*end MUST_MATCH*/

// Matches the current token but doesn't store it in a var:
#define MATCH_(type) \
    if (!match<type>()) { FAILURE; return {}; } \
    /*end MATCH_*/

//Same as MATCH_, but instead of returning a nullptr, throws an exception:
#define MUST_MATCH_(type) \
    if (!match<type>()) { FAILURE; \
    throw ParserException("Expected '" + type().toString() \
        + "' at position: " + to_string(_tokenPos)\
        + "\nPossible invalid expression or statement."); } \
    /*end MUST_MATCH_*/

ASNPtr deref(ASNPtr& p)
{
    return move(p);
}

BlockPtr deref(BlockPtr& p)
{
    return move(p);
}

template <typename T>
decltype(auto) deref(T& t)
{
    return *t;
}

// Calls <parser> and stores the result in <var>.
//  On success, var holds the result.
//  On nullopt, the present function returns early with nullptr:
#define PARSE(var, parser) \
    auto var##__ = parser; \
    if (!var##__) { FAILURE; return {}; } \
    decltype(auto) var = deref(var##__)
    /*end PARSE*/

// Same as PARSE, but throws a specified exception message if it fails:
#define MUST_PARSE(var, parser, exceptionMsg) \
    auto var##__ = parser; \
    if (!var##__) { FAILURE; \
    throw ParserException( String(exceptionMsg) \
        + " at position: " + to_string(_tokenPos)); } \
    decltype(auto) var = deref(var##__)
    /*end MUST_PARSE*/

// Parses a NameToken as a string.
Optional<String> Parser::parseName()
{
    TRACE;
    MATCH(nameToken, NameToken);
    SUCCESS;
    return nameToken.name;
}

Optional<ValueType> Parser::parseValueType()
{
    TRACE;
    MATCH(nameToken, NameToken);
    Vector<ValueType> tvars;

    if (match<LeftSquareToken>())
    {
        Optional<ValueType> tv = parseValueType();

        if (tv)
        {
            tvars.push_back(*tv);

            while (match<CommaToken>())
            {
                MUST_PARSE(tv1, parseValueType(), "Expected type after comma");
                tvars.push_back(tv1);
            }

            MUST_MATCH_(RightSquareToken);
        }
    }

    SUCCESS;
    return ValueType(nameToken.name, move(tvars));
}

Optional<OpType> Parser::parseUnaryOp()
{
    TRACE;
    switch (cur()->getType())
    {
        case tokNot:    SUCCESS; next(); return opNot;
        case tokMinus:  SUCCESS; next(); return opMinus;
        default:        FAILURE; return nullopt;
    }
}

Optional<OpType> Parser::parseMultiveOp()
{
    TRACE;
    switch (cur()->getType())
    {
        case tokMult:   SUCCESS; next(); return opMult;
        case tokDiv:    SUCCESS; next(); return opDiv;
        default:        FAILURE; return nullopt;
    }
}

Optional<OpType> Parser::parseAdditiveOp()
{
    TRACE;
    switch (cur()->getType())
    {
        case tokPlus:   SUCCESS; next(); return opPlus;
        case tokMinus:  SUCCESS; next(); return opMinus;
        default:        FAILURE; return nullopt;
    }
}

Optional<OpType> Parser::parseLogicalOp()
{
    TRACE;
    switch (cur()->getType())
    {
        case tokAnd:    SUCCESS; next(); return opAnd;
        case tokOr:     SUCCESS; next(); return opOr;
        case tokEq:     SUCCESS; next(); return opLogEq;
        case tokNotEq:  SUCCESS; next(); return opLogNotEq;
        default:        FAILURE; return nullopt;
    }
}

Optional<Variable> Parser::parseVariable()
{
    TRACE;
    ENABLE_ROLLBACK;

    if (match<ThisToken>())
    {
        if (match<MemberToken>())
        {
            // this.something
            PARSE(member, parseName());
            CANCEL_ROLLBACK;
            SUCCESS;
            return Variable("this", member);
        }
        else
        {
            // just this
            CANCEL_ROLLBACK;
            SUCCESS;
            return Variable(nullopt, "this");
        }
    }
    else
    {
        PARSE(object, parseName());

        if (match<MemberToken>())
        {
            // stuff.something
            PARSE(member, parseName());
            CANCEL_ROLLBACK;
            SUCCESS;
            return Variable(object, member);
        }
        else 
        {
            // just stuff
            // This could be just a variable, or a member with implicit this.
            CANCEL_ROLLBACK;
            SUCCESS;
            return Variable(nullopt, object);
        }
    }
}


// EXPRESSION PARSERS

ASNPtr Parser::parseVariableExp()
{
    TRACE;
    PARSE(var, parseVariable());
    SUCCESS;
    if (var.object)
    {
        return make_unique<VariableExp>(*var.object, var.variable);
    }
    else
    {
        return make_unique<VariableExp>(var.variable);
    }
}

Optional<FormalArg> Parser::parseFormalArg()
{
    TRACE;
    ENABLE_ROLLBACK;
    PARSE(type, parseValueType());
    PARSE(var, parseName());
    CANCEL_ROLLBACK;
    SUCCESS;
    return FormalArg{type, var};
}

ASNPtr Parser::parseNumber()
{
    TRACE;
    MATCH(num, NumberToken);
    SUCCESS;
    return make_unique<NumberExp>(num.num);
}

ASNPtr Parser::parseBoolTrue()
{
    TRACE;
    MATCH_(TrueToken);
    SUCCESS;
    return make_unique<BoolExp>(true);
}

ASNPtr Parser::parseBoolFalse()
{
    TRACE;
    MATCH_(FalseToken);
    SUCCESS;
    return make_unique<BoolExp>(false);
}

ASNPtr Parser::parseUnary()
{
    TRACE;
    // op prim
    ENABLE_ROLLBACK;

    PARSE(op, parseUnaryOp());
    MUST_PARSE(prim, parsePrimary(), "Expected expression after unary operator");

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<UnopExp>(move(prim), op);
}

ASNPtr Parser::parseMethodExp()
{
    // functionName + ( + exp + (, + exp)* + )
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> exps;
    ASNPtr temp;

    PARSE(method, parseVariable());
    MATCH_(LeftParenToken);
    temp = parseExp();
    if(temp)
    {
        exps.push_back(move(temp));
        while(match<CommaToken>())
        {
            MUST_PARSE(temp1, parseExp(), "Expected expression after ','");
            exps.push_back(move(temp1));
        }
    }
    MUST_MATCH_(RightParenToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<MethodExp>(move(method), move(exps));
}

ASNPtr Parser::parseNew()
{
    // new + type + ( + exp + (, + exp)* + )
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> exps;
    ASNPtr temp;

    MATCH_(NewToken);
    MUST_PARSE(type, parseValueType(), "type declaration for new");
    MATCH_(LeftParenToken);
    temp = parseExp();
    if(temp)
    {
        exps.push_back(move(temp));
        while(match<CommaToken>())
        {
            MUST_PARSE(temp1, parseExp(), "Expected expression after ','");
            exps.push_back(move(temp1));
        }
    }
    MUST_MATCH_(RightParenToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<NewExp>(move(type), move(exps));
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
    MUST_MATCH_(RightParenToken);

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
    else if (result = parseMethodExp())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseBoolTrue())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseBoolFalse())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseNew())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseVariableExp())
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
    MUST_PARSE(right, parseMultiveDown(), "Expected expression after multive operator");

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
    MUST_PARSE(right, parseAdditiveDown(), "Expected expression after additive operator");

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
    MUST_PARSE(right, parseLogicalDown(), "Expected expression after logical operator");

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

ASNPtr Parser::parseVarAssignDecl()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(type, parseValueType());
    PARSE(name, parseName());
    MATCH_(AssignToken);
    MUST_PARSE(exp, parseExp(), "Expected expression in assignment");
    MUST_MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<VarDecAssignStm>(move(type), move(name), move(exp));
}

ASNPtr Parser::parseVarDecl()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(type, parseValueType());
    PARSE(name, parseName());
    MUST_MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<VarDecStm>(move(type), move(name));
}

ASNPtr Parser::parseAssignStm()
{
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(lhs, parseVariableExp());
    MATCH_(AssignToken);
    MUST_PARSE(rhs, parseExp(), "Expected expression in assignment");
    MUST_MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<AssignStm>(move(lhs), move(rhs));
}

ASNPtr Parser::parseMethodStm()
{
    // functionName + ( + exp + (, + exp)* + ) + ;
    TRACE;
    ENABLE_ROLLBACK;

    PARSE(exp, parseMethodExp());
    MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<MethodStm>(move(exp));
}

ASNPtr Parser::parseIfStm()
{
    TRACE;

    ENABLE_ROLLBACK;
    MATCH_(IfToken);
    MUST_MATCH_(LeftParenToken);
    MUST_PARSE(logicExp, parseExp(), "Expected logical expression in if statement");
    MUST_MATCH_(RightParenToken);
    MUST_PARSE(trueStatements, parseBlock(), "Expected block{} after if statement");
    BlockPtr elseBlock;
    if( match<ElseToken>() )
    {
        MUST_PARSE(falseStatements, parseBlock(), "Expected block{} after else statement");
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
    MUST_MATCH_(LeftParenToken);
    MUST_PARSE(cond, parseExp(), "Expected logical expression in while statement");
    MUST_MATCH_(RightParenToken);
    MUST_PARSE(body, parseBlock(), "Expected block{} after while statement");

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<WhileStm>(move(cond), move(body));
}

ASNPtr Parser::parseStm()
{
    TRACE;
    ASNPtr result;

    if (result = parseVarAssignDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseVarDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseAssignStm())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseMethodStm())
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
    else if (result = parseMethodDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseRetStm())
    {
        SUCCESS;
        return result;
    }
//    else if (result = parseExp())     //only for testing
//    {
//        SUCCESS;
//        return result;
//    }
    else
    {
        FAILURE;
        return nullptr;
    }
}

// COMPOUND PARSERS

BlockPtr Parser::parseBlock()
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

    MUST_MATCH_(RightBraceToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<Block>(move(stm));
}

ASNPtr Parser::parseMethodDecl()
{
    /*
    type + name + ( + exp + (, + exp)* + ) + ;
    */
    TRACE;
    ENABLE_ROLLBACK;

    Vector<FormalArg> exps;
    Optional<FormalArg> temp;

    PARSE(retType, parseValueType());
    PARSE(name, parseName());
    MATCH_(LeftParenToken);
    temp = parseFormalArg();
    if(temp)
    {
        exps.push_back(*temp);
        while(match<CommaToken>())
        {
            MUST_PARSE(temp1, parseFormalArg(), "Expected type variable after ','");
            exps.push_back(temp1);
        }
    }
    MUST_MATCH_(RightParenToken);
    MUST_PARSE(body, parseBlock(), "Expected block{} for method body");

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<MethodDef>(
            move(retType), move(name), move(exps), move(body)
            );
}

ASNPtr Parser::parseConsDecl()
{
    // Constructor. Like a method, but instead of beginning with
    //  <TYPE> <NAME>, just begins with <CONS>
    TRACE;
    ENABLE_ROLLBACK;

    Vector<FormalArg> exps;
    Optional<FormalArg> temp;

    MATCH_(ConsToken);
    MUST_MATCH_(LeftParenToken);
    temp = parseFormalArg();
    if (temp)
    {
        exps.push_back(*temp);
        while(match<CommaToken>())
        {
            MUST_PARSE(temp1, parseFormalArg(), "Expected type variable after ','");
            exps.push_back(temp1);
        }
    }
    MUST_MATCH_(RightParenToken);
    MUST_PARSE(body, parseBlock(), "Expected block{} for constructor body");

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<ConsDef>(move(exps), move(body));
}

ASNPtr Parser::parseClassDecl()
{
    TRACE;
    ENABLE_ROLLBACK;

    Vector<ASNPtr> stm;
    ASNPtr curstm = nullptr;
    ClassDecl* parent = nullptr;

    MATCH_(ClassToken);
    MUST_PARSE(classType, parseValueType(), "Expected class type");

    // Parse optional parent class.
    if (match<ExtendsToken>())
    {
        MUST_PARSE(parentType, parseValueType(), "Expected base class type");
        parent = _classes[parentType];
        if(!parent) 
        {
            throw ParserException("Undeclared Base class: " + parentType.toString());
        }
    }

    MUST_MATCH_(LeftBraceToken)

    // Parse class body.
    while(curstm = parseClassStm())
    {
        stm.push_back(move(curstm));
    }
    MUST_MATCH_(RightBraceToken);
    MUST_MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    auto result = make_unique<ClassDecl>(classType, move(stm), move(parent));
    _classes.insert({ classType, result.get() });
    return result;
}

ASNPtr Parser::parseClassStm()
{
    TRACE;
    ASNPtr result;

    if (result = parseConsDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseMethodDecl())
    {
        SUCCESS;
        return result;
    }
    else if (result = parseVarDecl())
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

ASNPtr Parser::parseRetStm()
{
    TRACE;
    ENABLE_ROLLBACK;

    MATCH_(ReturnToken);
    MUST_PARSE(exp, parseExp(), "Expected expression for return statement");
    MUST_MATCH_(SemiToken);

    CANCEL_ROLLBACK;
    SUCCESS;
    return make_unique<RetStm>(move(exp));
}

Vector<ASNPtr> Parser::parseProgram()
{
    TRACE;
    Vector<ASNPtr> prog;
    bool run = true;
    ASNPtr result;

    while(run)
    {
        if(result = parseClassDecl())
        {
            prog.push_back(move(result));
        }
        else
        {
            run = false;
        }
    }

    if(!match<EndToken>())
    {
        FAILURE;
        String msg = "Unable to parse at position: " + to_string(_tokenPos)
                + "\nUnexpected: " + _tokens[_tokenPos]->toString();
        throw ParserException(msg);
    }

    return prog;
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
    if (config::traceParse)
    {
        std::cout << "\n";
        _tracer.finalize();
        _tracer.print();
    }
}

} //namespace dflat
