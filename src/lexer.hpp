#ifndef LEXER_HPP
#define LEXER_HPP

#include "lexercore.hpp"
#include "vector.hpp"
#include "token.hpp"

namespace dflat
{

Vector<TokenPtr> tokenize(String const&);

class LexerException: public std::runtime_error
{
public:
    LexerException(String msg) noexcept;
};

} //namespace dflat

#endif // LEXER_HPP
