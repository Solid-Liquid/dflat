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
        //TokenPtr singleToken();
        void skipWhitespace();
        TokenPtr lookupKeyword(String const&);
};

Lexer::Lexer(String const& input)
    : LexerCore(input)
{
}

TokenPtr Lexer::lookupKeyword(String const& name)
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

//     // if(){
        
//     // }

//     return nullptr;
// }

Vector<TokenPtr> Lexer::tokenize()
{
    Vector<TokenPtr> tokens;
    // TODO everything
    TokenPtr current = nullptr;

    // while( (current = singleToken()) !=  nullptr) {
    //     tokens.push_back(current);
    // }
    

    return tokens;
}

Vector<TokenPtr> tokenize(String const& input)
{
    Lexer lexer(input);
    return lexer.tokenize();
}

} //namespace dflat
