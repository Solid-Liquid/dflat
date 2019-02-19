#include "lexer.hpp"

namespace dflat
{


class Lexer : private LexerCore
{
    public:
        Lexer(String const&);
        Vector<TokenPtr> tokenize();
        TokenPtr singleToken();
        TokenPtr tryTokenizeVariable();
        TokenPtr tryTokenizeNumber();
};

Lexer::Lexer(String const& input)
    : LexerCore(input)
{
}

TokenPtr Lexer::tryTokenizeVariable()
{

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
