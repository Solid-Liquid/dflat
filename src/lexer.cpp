#include "lexer.hpp"
#include "map.hpp"

namespace dflat
{
    using namespace std;

class Lexer : private LexerCore
{
    public:
        Lexer(String const&);
        Vector<TokenPtr> tokenize();
        TokenPtr singleToken();
        TokenPtr tryTokenizeVariable();
        TokenPtr tryTokenizeNumber();
        //TokenPtr singleToken();
        void skipWhitespace();
        TokenPtr lookupKeyword(String const&) const;
        TokenPtr lookupPunct(char c) const;
};

Lexer::Lexer(String const& input)
    : LexerCore(input)
{
}

TokenPtr Lexer::tryTokenizeVariable()
{
    char c = peek();
    String var = "";

    if (!isalpha(c))
    {
        return nullptr;
    }

    while (isalpha(c) || isdigit(c))
    {
        var += get();
        c = peek();
    }

    if(var.length() == 0)
        return nullptr;
    
    TokenPtr ret = lookupKeyword(var);

    if(ret == nullptr)
        return make_unique<VariableToken>(var);

    return ret;
}

TokenPtr Lexer::tryTokenizeNumber()
{
    const size_t initialInputPos = _pos;
    String digits = "";

    char index = peek();
    while ( index && isdigit(index) )
    {
        digits += index;
        next();
        index = peek();
    }

    if ( digits.length() > 0)
    {
        return make_unique<NumberToken>(stoi(digits));
    }
    else
    {
        _pos = initialInputPos;
        return nullptr;
    }
}

TokenPtr Lexer::lookupKeyword(String const& name) const
{
    static Map<String, TokType> const kws
    {
        { "if", tokIf },
        { "else", tokElse },
        { "for", tokFor },
        { "while", tokWhile },
    };

    auto it = kws.find(name);

    if (it == kws.end())
    {
        return nullptr;
    }

    switch (it->second)
    {
        case tokIf: return make_unique<IfToken>();
        case tokElse: return make_unique<ElseToken>();
        case tokFor: return make_unique<ForToken>();
        case tokWhile: return make_unique<WhileToken>();
        default: return nullptr;
    }
}

TokenPtr Lexer::lookupPunct(char c) const
{
    switch (c)
    {
        case '(': return make_unique<LeftParenToken>();
        case ')': return make_unique<RightParenToken>();
        case '{': return make_unique<LeftBraceToken>();
        case '}': return make_unique<RightBraceToken>();
        case '=': return make_unique<EqualToken>();
        case '+': return make_unique<PlusToken>();
        case '-': return make_unique<MinusToken>();
        case '*': return make_unique<MultiplyToken>();
        case '/': return make_unique<DivisionToken>();
        case '&': return make_unique<AndToken>();
        case '|': return make_unique<OrToken>();
        case '!': return make_unique<NotToken>();
        case '\n': return make_unique<NewlineToken>();
        default: return nullptr;
    }
}

void Lexer::skipWhitespace()
{
    char c = peek();

    while (c == ' ' || c == '\t') // no \n
    {
        next();
        c = peek();
    }
}

// TokenPtr Lexer::singleToken(){
//     TokenPtr tok = nullptr;


TokenPtr Lexer::singleToken(){
    TokenPtr tok = nullptr;

    if (tok = tryTokenizeVariable())
    {
        return tok;
    } 
    else if (tok = tryTokenizeNumber())
    {
        return tok;
    }
    else if (tok = lookupPunct(peek()))
    {
        next();
        return tok;
    }

    return nullptr;
}

Vector<TokenPtr> Lexer::tokenize()
{
    Vector<TokenPtr> tokens;
    TokenPtr current = nullptr;
    skipWhitespace();

    while(current = singleToken()) 
    {
        tokens.push_back(move(current));
        skipWhitespace();
    }

    return tokens;
}

Vector<TokenPtr> tokenize(String const& input)
{
    Lexer lexer(input);
    return lexer.tokenize();
}

} //namespace dflat
