#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexercore.hpp"
#include "vector.hpp"
#include "token.hpp"

namespace dflat
{

Vector<TokenPtr> tokenize(String const&);

class LexerException: public std::exception
{
public:
    LexerException(String msg) noexcept;
    const char* what() const noexcept;
private:
    String message;
};

} //namespace dflat

#endif // LEXER_HPP
