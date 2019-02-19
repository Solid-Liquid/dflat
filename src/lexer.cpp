#include "lexer.hpp"

namespace dflat
{


class Lexer : private LexerCore
{
    public:
        Lexer(String const&);
        Vector<TokenPtr> tokenize();
        //TokenPtr singleToken();
};

Lexer::Lexer(String const& input)
    : LexerCore(input)
{
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
