#include "lexer.hpp"
#include "map.hpp"

namespace dflat
{
    using std::make_unique;

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

    while((c >= 'A' && c <= 'Z') || (c >= 'a' && c < 'z' ))
    {
        var += get();
        c = peek();
    }

    // see if var is contained in enum
    // if in enum, return that specific token
    // else return variable token
    return std::make_unique<VariableToken>(var);
}

TokenPtr Lexer::tryTokenizeNumber()
{
    const int initialInputPos = _pos;
    String digits = "";

        while (char index = peek() && isdigit(index) ) { 
        digits += index;
        next();
    }

    if ( digits.length > 0)
    {
        return std::make_unique<NumberToken>(std::stoi(digits));
    } else 
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
        case '+': return make_unique<PlusToken>();
        case '-': return make_unique<MinusToken>();
        case '*': return make_unique<MultiplyToken>();
        case '/': return make_unique<DivisionToken>();
        case '&': return make_unique<AndToken>();
        case '|': return make_unique<OrToken>();
        case '!': return make_unique<NotToken>();
        default: return nullptr;
    }
}

void Lexer::skipWhitespace()
{
    char c = peek();

    while (c == ' ' || c == '\t') // no \n
    {
        c = get();
    }
}

// TokenPtr Lexer::singleToken(){
//     TokenPtr tok = nullptr;


TokenPtr Lexer::singleToken(){
    TokenPtr tok = nullptr;

    if (tok = tryTokenizeVariable())
    {
        return tok;
    } else if (tok = tryTokenizeNumber())
    {
        return tok;
    }

    return nullptr;
}

Vector<TokenPtr> Lexer::tokenize()
{
    Vector<TokenPtr> tokens;
    TokenPtr current = nullptr;

    while( (current = singleToken()) !=  nullptr) {
        tokens.push_back(current);
    }
    

    return tokens;
}

Vector<TokenPtr> tokenize(String const& input)
{
    Lexer lexer(input);
    return lexer.tokenize();
}

} //namespace dflat
